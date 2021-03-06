% This file is part of the 'foreign' package for R
% It is distributed under the GPL version 2 or later

\name{S3 read functions}
\alias{data.restore}
\alias{read.S}
\title{Read an S3 Binary or data.dump File}
\description{
  Reads binary data files or \code{data.dump} files that were produced
  in S version 3.
}
\usage{
  data.restore(file, print = FALSE, verbose = FALSE, env = .GlobalEnv)
  read.S(file)
}
\arguments{
  \item{file}{the filename of the S-PLUS \code{data.dump} or binary
    file.}
  \item{print}{whether to print the name of each object as read from the
    file.}
  \item{verbose}{whether to print the name of every subitem within each
    object.}
  \item{env}{environment within which to create the restored object(s).}
}
\value{
  For \code{read.S}, an R version of the S3 object.

  For \code{data.restore}, the name of the file.
}
\details{
  \code{read.S} can read the binary files produced in some older
  versions of S-PLUS on either Windows (versions 3.x, 4.x, 2000) or Unix
  (version 3.x with 4 byte integers).  It automatically detects whether
  the file was produced on a big- or little-endian machine and adapts
  itself accordingly.

  \code{data.restore} can read a similar range of files produced by
  \code{data.dump} and for newer versions of S-PLUS, those from
  \code{data.dump(....., oldStyle=TRUE)}.

  Not all S3 objects can be handled in the current version.  The most
  frequently encountered exceptions are functions and expressions; you
  will also have trouble with objects that contain model formulas.  In
  particular, comments will be lost from function bodies, and the
  argument lists of functions will often be changed.
}
\author{
  Duncan Murdoch
}
\examples{
\dontrun{read.S(file.path("_Data", "myobj"))
data.restore("dumpdata", print = TRUE)
}}
\keyword{data}
\keyword{file}
