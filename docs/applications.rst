libqcpp Applications
====================

Libqcpp ships with some applications built using the library.


.. contents::


Trimit
^^^^^^

Trimit ties together several common QC measures applied to short read
sequencing data. It works with paired end Illumina and similar sequencing
experiemnts.


QC Steps
--------

- Measure per-base quality scores
- Trim/Merge reads: does a global alignment between read pairs to detect
  read-through. Read pairs from fragments less than the read length are trimmed
  at the fragment length, discarding the second read. Read pairs from fragments
  that are longer than the read length but less than twice the read length are
  merged. Read pairs from fragments longer than twice the read length are not
  modified.
- Windowed quality control: a sliding-window based quality score trimmer, which
  uses an improved version of the `sickle <https://github.com/najoshi/sickle>`_
  trimming algorithm.

Usage
-----

See `trimit -h`.
