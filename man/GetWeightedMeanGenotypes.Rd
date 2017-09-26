\name{GetWeightedMeanGenotypes}
\alias{GetWeightedMeanGenotypes}
\alias{GetWeightedMeanGenotypes.RADdata}
\title{
Export Numeric Genotype Values Weighted by Posterior Probabilities
}
\description{
For each allele in a \code{"\link{RADdata}"} object, this function
selects the best-fitting ploidy from the \code{$ploidyChiSq} slot, 
and exports numerical genotype values calculated from the posterior
genotype probabilities for that ploidy.  A matrix is returned with
numerical genotype values for each taxon and allele.
}
\usage{
GetWeightedMeanGenotypes(object, ...)
\method{GetWeightedMeanGenotypes}{RADdata}(object, minval = 0, maxval = 1, 
                         omit1allelePerLocus = TRUE, 
                         naIfZeroReads = FALSE, \dots)
}
\arguments{
  \item{object}{
A \code{"\link{RADdata}"} object.  Posterior genotype probabilities should
have been added with \code{\link{AddGenotypePosteriorProb}}, and if there is
more than one possible ploidy,
ploidy chi-squared values should have been added with 
\code{\link{AddPloidyChiSq}}.
}
  \item{\dots}{
Additional arguments, listed below, to be passed to the method for
\code{"RADdata"}.
}
\item{minval}{
The number that should be used for indicating that a taxon has zero
copies of an allele.
}
\item{maxval}{
The number that should be used for indicating that a taxon has the
maximum copies of an allele (equal to the ploidy of the locus).
}
\item{omit1allelePerLocus}{
A logical indicating whether one allele per locus should be omitted
from the output, in order to reduce the number of variables and
prevent singularities for 
genome-wide association and genomic prediction.  The value for one
allele can be predicted from the values from all other alleles at
its locus.
}
\item{naIfZeroReads}{
A logical indicating whether \code{NA} should be inserted into the 
output matrix for any taxa and loci where the total read depth for
the locus is zero.  If \code{FALSE}, the output for these genotypes is
essentially the weighted mean across prior genotype probabilities, since
prior and posterior genotype probabilities are equal when there are no
reads.
}
}

\value{
A named matrix, with taxa in rows and alleles in columns,
and values ranging from \code{minval} to \code{maxval}.
These values can be treated as continuous genotypes.
}
\author{
Lindsay V. Clark
}

\examples{
# load dataset and set some parameters
data(exampleRAD_mapping)
exampleRAD_mapping <- SetDonorParent(exampleRAD_mapping, "parent1")
exampleRAD_mapping <- SetRecurrentParent(exampleRAD_mapping, "parent2")
exampleRAD_mapping <- AddAlleleFreqMapping(exampleRAD_mapping,
                                           expectedFreqs = c(0.25, 0.75),
                                           allowedDeviation = 0.08)
exampleRAD_mapping <- AddGenotypeLikelihood(exampleRAD_mapping)
exampleRAD_mapping <- AddGenotypePriorProb_Mapping2Parents(exampleRAD_mapping,
                                                      n.gen.backcrossing = 1)
exampleRAD_mapping <- AddGenotypePosteriorProb(exampleRAD_mapping)
exampleRAD_mapping <- AddPloidyChiSq(exampleRAD_mapping)

# get weighted mean genotypes
wmg <- GetWeightedMeanGenotypes(exampleRAD_mapping)
# examine the results
wmg[1:10,]
}

\keyword{ methods }
\keyword{ array }