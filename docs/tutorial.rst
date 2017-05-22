Trimit usage tutorial
=====================

.. contents::


Quality control on Arabidopsis reads
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This example is from the `1001 genomes project
<http://www.cell.com/cell/abstract/S0092-8674%2816%2930667-5>`_. We will
operate on a small set of reads extracted from one sample (SRR1945463).

First, we need to download and extract the prepared data.

.. code-block:: shell

    wget -O - https://github.com/kdmurray91/libqcpp/raw/master/docs/tutorial-data.tar.gz | tar xzv

The following files should have been created:

- ``first-1000.fastq``: The first 1000 read pairs from this sample.
- ``has-adaptors.fastq``: A read pair whose insert size is less than the read
  length, and hence has adaptors in reads.
- ``trimmable.fastq``: a read pair with low base quality sequences
- ``mergeable.fastq``: a read pair that can be merged


Now, we will QC the first 1000 read pairs using trimit

.. code-block:: shell

    trimit first-1000.fastq > first-1000-defaults.fastq


That command uses the default values for minimum quality score (25) and minimum read
length of reads (1bp, i.e. no filtering). These can both be adjusted:

.. code-block:: shell

    trimit -q 30 -l 50 first-1000.fastq > first-1000-q30l50.fastq


QC measures
-----------

The files ``has-adaptors.fastq``, ``trimmable.fastq`` and ``mergeable.fastq``
demonstrate the main modes of operation for trimit.

.. code-block:: shell

    # This read pair's insert size is quite small, meaning that there are
    # adaptors in the sequences. This results in a single small read containing
    # the consensus of the overlapping reads.

    trimit has-adaptors.fastq


    # This read pair's insert size is larger than the read length, but smaller
    # than twice the read length. This means the pair can be merged into a single
    # fragment-length read.

    trimit mergeable.fastq


    # This read pair has reads with 3' ends whose base quality is low, and are therefore
    # trimmed from the reads. This results in two shorter reads.

    trimit trimable.fastq



QC reports
----------

Trimit (and libqcpp) can prepare YAML-formatted reports on QC processing steps.


.. code-block:: shell

    trimit -q 30 -l 50  -y report.yml first-1000.fastq > first-1000-q30l50.fastq

    less report.yml

This report contains details of all processing steps, including a summary of
the number of reads trimmed and merged, and of the per-cycle quality of all
reads.


QC-ing the whole sample
-----------------------

If you wish to QC the entire sample these reads come from, please use the
following commands.

.. code-block:: shell

    wget -O reads.sra https://sra-download.ncbi.nlm.nih.gov/srapub/SRR1945463

    # Dump a fastq file
    fastq-dump \
        --split-spot \
        --skip-technical \
        --stdout \
        --readids \
        --defline-seq '@$sn/$ri' \
        --defline-qual '+' \
        reads.sra > reads.fastq

