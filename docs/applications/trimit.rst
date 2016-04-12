..
  Copyright 2015 Kevin Murray <spam@kdmurray.id.au>
  .
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  .
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  .
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.


Trimit
======

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
