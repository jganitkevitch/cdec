#!/bin/bash
#This is just an example compilation.  You should integrate these files into your build system.  I can provide boost jam if you want.  
#If your code uses ICU, edit util/string_piece.hh and uncomment #define USE_ICU
#I use zlib by default.  If you don't want to depend on zlib, remove #define USE_ZLIB from util/file_piece.hh

set -e

for i in util/{bit_packing,ersatz_progress,exception,file_piece,murmur_hash,scoped,mmap} lm/{bhiksha,binary_format,config,lm_exception,model,quantize,read_arpa,search_hashed,search_trie,trie,virtual_interface,vocab}; do
  g++ -I. -O3 $CXXFLAGS -c $i.cc -o $i.o
done
g++ -I. -O3 $CXXFLAGS lm/build_binary.cc {lm,util}/*.o -lz -o build_binary
g++ -I. -O3 $CXXFLAGS lm/ngram_query.cc {lm,util}/*.o -lz -o query
