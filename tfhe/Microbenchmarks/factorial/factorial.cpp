#include <iostream>
#include <fstream>

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <tfhe/tfhe_generic_streams.h>

#include "../../helper.hpp"

// Compute and return the enc_idx factorial number. Run until max_id.
LweSample* fact(const LweSample* enc_idx, const uint32_t max_idx,
                const uint32_t word_sz,
                const TFheGateBootstrappingCloudKeySet* bk) {
  // Initialize result to Enc(0).
  LweSample* result =
    new_gate_bootstrapping_ciphertext_array(word_sz, bk->params);
  // Initialize with 1.
  bootsCONSTANT(&result[0], 1, bk);
  for (int i = 1; i < word_sz; i++) {
    bootsCONSTANT(&result[i], 0, bk);
  }
  // TODO(@cgouert): maybe this should be more than 32 bits?
  uint32_t fact = 1;
  for (int i = 1; i <= max_idx; i++) {
    fact *= i;
    LweSample* curr_idx = enc_cloud(i, word_sz, bk);
    LweSample* fact_ctxt = enc_cloud(fact, word_sz, bk);
    LweSample* control = cmp(curr_idx, enc_idx, word_sz, bk);
    for (int j = 0; j < word_sz; j++) {
      bootsMUX(&result[j], &control[0], &result[j], &fact_ctxt[j], bk);
    }
  }
  return result;
}

int main(int argc, char** argv) {
  // Argument sanity checks.
  std::ifstream cloud_key, ctxt_file;
  int word_sz = 0, max_index = 0;
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0] <<
      " cloud_key ctxt_filename wordsize max_index" << std::endl <<
      "\tcloud_key: Path to the secret key" <<  std::endl <<
      "\tctxt_filename: Path to the ciphertext file" << std::endl <<
      "\twordsize: Number of bits per encrypted int" << std::endl <<
      "\tmax_index: Maximum number of iterations" << std::endl;
    return EXIT_FAILURE;
  } else {
    // Check if secret key file exists.
    cloud_key.open(argv[1]);
    if (!cloud_key) {
      std::cerr << "file " << argv[1] << " does not exist" << std::endl;
      return EXIT_FAILURE;
    }
    // Check if ciphertext file exists.
    ctxt_file.open(argv[2]);
    if (!ctxt_file) {
      std::cerr << "file " << argv[2] << " does not exist" << std::endl;
      return EXIT_FAILURE;
    }
    // Check wordsize.
    word_sz = atoi(argv[3]);
    if (word_sz < 8 || word_sz > 64 || !is_pow_of_2(word_sz)) {
      std::cerr << "wordsize should be a power of 2 in the range 2^3 - 2^6"
        << std::endl;
      return EXIT_FAILURE;
    }
    // Check max index.
    max_index = atoi(argv[4]);
    if (max_index < 0 || max_index > 1000) {
      std::cerr << "Maximum iterations should be in [0, 1000]"<< std::endl;
      return EXIT_FAILURE;
    }
  }
  TFheGateBootstrappingCloudKeySet* bk =
    new_tfheGateBootstrappingCloudKeySet_fromStream(cloud_key);
  cloud_key.close();

  // If necessary, the params are inside the key.
  const TFheGateBootstrappingParameterSet* params = bk->params;

  // Read the ciphertext objects.
  uint32_t num_ctxts = 1;
  ctxt_file >> num_ctxts;
  LweSample* user_data =
    new_gate_bootstrapping_ciphertext_array(word_sz, params);
  for (int j = 0; j < word_sz; j++) {
    import_gate_bootstrapping_ciphertext_fromStream(ctxt_file, &user_data[j], params);
  }
  ctxt_file.close();

  LweSample* enc_result = fact(user_data, max_index, word_sz, bk);

  // Output result(s) to file.
  std::ofstream ctxt_out("output.ctxt");
  // The first line of ptxt_file contains the number of lines.
  ctxt_out << 1;
  for (int j = 0; j < word_sz; j++) {
    export_lweSample_toStream(ctxt_out, &enc_result[j], params->in_out_params);
  }
  delete_gate_bootstrapping_ciphertext_array(word_sz, enc_result);
  ctxt_out.close();

  // Clean up all pointers.
  delete_gate_bootstrapping_ciphertext_array(word_sz, user_data);
  delete_gate_bootstrapping_cloud_keyset(bk);
  return EXIT_SUCCESS;
}
