#include <Rcpp.h>
using namespace Rcpp;

// Function to take a 3D matrix of genotype likelihoods or probabilities
// (as a vector) and output a matrix of the best genotypes.
// [[Rcpp::export]]
IntegerMatrix BestGenos(NumericVector probs, int ploidy, int ntaxa, int nalleles) {
  IntegerMatrix bestgenos(ntaxa, nalleles);
  int nprobs = probs.size();
  int ngen = ploidy + 1;
  int ngentimestaxa = ngen * ntaxa;
  int bestgen = NA_INTEGER;
  float bestprob = 0;
  int copynum;
  int taxon;
  int allele;
  
  for(int i = 0; i < nprobs; i++){
    copynum = i % ngen;
    taxon = i / ngen % ntaxa;
    allele = i / ngentimestaxa;
    
    if(copynum > 0 && probs[i] == bestprob){
      bestgen = NA_INTEGER; // NA if there is a tie
    }
    if(copynum == 0 || probs[i] > bestprob){
      bestgen = copynum;    // Update or start new with best genotype
      bestprob = probs[i];
      if(NumericVector::is_na(probs[i])){
        bestgen = NA_INTEGER; // NA if probs are missing
        bestprob = 0;
      }
    }
    if(copynum == ploidy){
      bestgenos(taxon, allele) = bestgen; // Output before moving to next
    }
  }
  
  return bestgenos;
}

// Internal function for getting 1D index for a matrix from 2D index
int RCto1D(int nrow, int row, int col) {
  int out = col * nrow + row;
  return out;
}

// Function to determine (roughly) the most probable multiallelic genotype from
// a set of pseudo-biallelic genotype probabilities.
// Works on one individual * locus.
// choose is how many alleles to choose, which starts at ploidy and is reduced
// to zero by recursion.
// [[Rcpp::export]]
List BestMultiGeno(NumericVector probs, int ploidy, int nalleles, int choose) {
  // initialize at all zeros, which is what will be returned if no alleles can
  // be picked
  IntegerVector outgeno(nalleles);
  double bestprob = 0;
  double thisprob;
  int p1 = ploidy + 1;
  NumericVector probssub;
  List out;
  List subres;
  double subres_bestprob;
  IntegerVector subres_outgeno;
  
  if(nalleles == 1){
    // if only one allele remains, all remaining copies must be this allele
    outgeno[0] = choose;
    bestprob = probs[RCto1D(p1, choose, 0)];
  }
  if(nalleles > 1 && choose == 0){
    // if none could be chosen, get the prob of zero for everything
    bestprob = 1;
    for(int a = 0; a < nalleles; a++){
      bestprob *= probs[RCto1D(p1, 0, a)];
    }
  }
  if(nalleles > 1 && choose > 0){
    // remove first allele from probabilities
    probssub = probs[Range(RCto1D(p1, 0, 1),
                           RCto1D(p1, ploidy, nalleles - 1))];
    
    // loop through possible copy numbers for first allele
    for(int i = 0; i < choose + 1; i++){
      thisprob = probs[RCto1D(p1, i, 0)];
      subres = BestMultiGeno(probssub, ploidy, nalleles - 1, choose - i);
      subres_bestprob = subres["bestprob"];
      thisprob *= subres_bestprob;
      if(thisprob > bestprob){
        bestprob = thisprob;
        outgeno[0] = i;
        subres_outgeno = subres["outgeno"];
        for(int a = 0; a < nalleles - 1; a++){
          outgeno[a + 1] = subres_outgeno[a];
        }
      }
    }
  }
  
  out["outgeno"] = outgeno;
  out["bestprob"] = bestprob;
  
  return out;
}

// Function to find best ploidies from ploidyChiSq slot
// [[Rcpp::export]]
IntegerVector BestPloidies(NumericMatrix chisq) {
  int nalleles = chisq.ncol();
  int npld = chisq.nrow();
  IntegerVector bestploidies(nalleles);
  int bestpld;
  float bestchisq;
  
  for(int a = 0; a < nalleles; a++){
    bestpld = 0;
    bestchisq = chisq(0,a);
    for(int pld = 0; pld < npld; pld++){
      if(chisq(pld, a) < bestchisq || (NumericVector::is_na(bestchisq) && !NumericVector::is_na(chisq(pld, a)))){
        bestpld = pld;
        bestchisq = chisq(pld, a);
      }
    }
    if(NumericVector::is_na(bestchisq)){
      bestpld = -1; // Zero in the output will indicate they were all missing.
    }
    bestploidies[a] = bestpld + 1;
  }
  
  return bestploidies;
}
