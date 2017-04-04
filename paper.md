---
title: 'libqcpp: A C++14 sequence quality control library'
tags:
- next-generation sequencing
- c++
- bioinformatics
authors:
  - name: Kevin D Murray
    orcid: 0000-0002-2466-1917
    affiliation: 1
  - name: Justin O Borevitz
    orcid: TBD
    affiliation: 1
affiliations:
  - name: >
      ARC Centre of Excellence in Plant Energy Biology,
      The Australian National University, Canberra, ACT 2602, Australia
    index: 1
date: 03 April 2017
bibliography: paper.bib
geometry:
    - margin=1in
---

# Summary

Libqcpp implements a variety of algorithms for Next-generation Sequencing (NGS)
data quality control. These algorithms include:

- Sliding-window quality score trimming, using an algorithm based on Sickle
  [@joshi11_sickle].
- A combined adaptor removal and read merging algorithm for paired end reads
  that uses global pairwise alignment of reads. This algorithm is similar to
  AdapterRemoval [@lindgreen12_adapterremoval].
- Cycle-wise summarisation of base quality scores, similar to FastQC
  [@andrews12_fastqc]

Libqcpp allows simple composition of quality control pipelines that combine
these features into a single unit. Application code can then simply read from a
stream of sequence reads that have passed quality control measures.
Optionally, parsing and quality control can occur in one or more background
threads for efficiency. Reports detailing actions performed and summaries of
results may be obtained in YAML format. Libqcpp includes `trimit`, a command
line interface to these features for those not building their own applications.

Libqcpp uses the SeqAn library for sequence parsing and alignment
[@doring08_seqan], libyaml-cpp for YAML report generation, and Catch for unit
testing. Documentation on API and command line usage is included, and available
at https://qcpp.readthedocs.io/.

# References
