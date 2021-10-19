#include <iostream>
#include <fstream>

#include <tfhe/tfhe.h>
#include <tfhe/tfhe_io.h>
#include <tfhe/tfhe_generic_streams.h>

#include "../../helper.hpp"

// Compute and return the Nth factorial.
LweSample* fact(LweSample* prev_fact_, LweSample* start_num_,
  const uint32_t N, const uint32_t word_sz, 
  const TFheGateBootstrappingCloudKeySet* bk) {
  // Initialize result to Enc(0).
  LweSample* result_ =
    new_gate_bootstrapping_ciphertext_array(word_sz, bk->params);
  for (int i = 0; i < word_sz; i++) {
    bootsCOPY(&result_[i], &prev_fact_[i], bk);
  }
  for (int i = 0; i < N; i++) {
    result_ = multiplier(result_, start_num_, word_sz, bk);
    start_num_ = incrementer(start_num_, word_sz, bk);
  }
  return result_;
}

int main(int argc, char** argv) {
  // Argument sanity checks.
  std::ifstream cloud_key, ctxt_file;
  int word_sz = 0, N = 0;
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0] <<
      " cloud_key ctxt_filename wordsize max_index" << std::endl <<
      "\tcloud_key: Path to the secret key" <<  std::endl <<
      "\tctxt_filename: Path to the ciphertext file" << std::endl <<
      "\twordsize: Number of bits per encrypted int" << std::endl <<
      "\tN: Number of iterations" << std::endl;
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
    N = atoi(argv[4]);
    if (N < 0 || N > 1000) {
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
  LweSample* user_data[num_ctxts];
  for (int i = 0; i < num_ctxts; i++) {
    user_data[i] = new_gate_bootstrapping_ciphertext_array(word_sz, params);
    for (int j = 0; j < word_sz; j++) {
      import_gate_bootstrapping_ciphertext_fromStream(ctxt_file, &user_data[i][j], params);
    }
  }
  ctxt_file.close();

  LweSample* enc_result = fact(user_data[0], user_data[1], N, word_sz, bk);

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
  for (int i = 0; i < num_ctxts; i++) {
    delete_gate_bootstrapping_ciphertext_array(word_sz, user_data[i]);
  }
  delete_gate_bootstrapping_cloud_keyset(bk);
  return EXIT_SUCCESS;
}
