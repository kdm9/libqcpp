libqcpp API
===========

.. contents::


Overview
--------

libqcpp's API is built around two concepts: Stream and Processors.


Streams
-------

``ReadStreams`` are streams of sequence reads. The in the basic case, these
streams parse reads from or write reads to a file or stream without
manipulation (``ReadInputStream`` and ``ReadOutputStream``). Streams that
process reads using some pipeline of processors have been implemented
(``ProcessedReadStream``). A high-level, multi-threaded read processor is also
provided (``ThreadedQCProcessor``). Streams can report statistics reads that
that have parsed or written as member variables or as a YAML report.

Processors
----------

Processors mutate or calculate statistics on a read or read pair. They may also
report statistics on all reads they have processed in their lifetime, as member
variables or as YAML reports.

The following processors are implemented (shown with constructor arguments).


``AdaptorTrimPE``
^^^^^^^^^^^^^^^^^

.. code::

   AdaptorTrimPE(const std::string &name, int min_overlap=10,
                 const QualityEncoding &encoding=SangerEncoding);

Aligns a read pair to each other, and detect either adapter read-through, or
read overlap. Operates only on paired reads. Yields single ended reads if the
read pair is either shorter than the read length (thus each read contains
adaptor sequence) or the read ends overlap.

``WindowedQCTrim``
^^^^^^^^^^^^^^^^^^

.. code::

   WindowedQualTrim(const std::string &name, int8_t min_quality,
                    size_t min_length, size_t window_size=0,
                    const QualityEncoding &encoding=SangerEncoding);

Uses a sliding-window based approach to trim reads at the point where base
quality decreases below a threshold. Reads are trimmed at the first position at
which a window's mean base quality is below ``min_quality``. The 5' end of a
read is also trimmed. Reads less than ``min_length`` bases long are removed
from the stream. The window size may be set using ``window_size``; a
``window_size`` value of 0 causes the window length to be 10% of the read
length.


``PerBaseQuality``
^^^^^^^^^^^^^^^^^^

.. code::

   PerBaseQuality(const std::string &name,
                  const QualityEncoding &encoding=SangerEncoding);

Records statistics on per-cycle quality across all read sets, reporting the
distribution of base quality scores for each cycle.


``ReadLenFilter``
^^^^^^^^^^^^^^^^^

.. code::

   ReadLenFilter(const std::string &name,
                 size_t             threshold = 1,
                 const QualityEncoding &encoding=SangerEncoding);

Filters reads less than ``threshold`` bases out of a stream.


``ReadLenCounter``
^^^^^^^^^^^^^^^^^^

.. code::

   ReadLenCounter(const std::string &name,
                  const QualityEncoding &encoding=SangerEncoding);

Counts the length distribution of all reads.


``ReadTruncator``
^^^^^^^^^^^^^^^^^

.. code::

   ReadTruncator(const std::string &name,
                 size_t threshold=64,
                 const QualityEncoding &encoding=SangerEncoding);

Truncates reads to ``threshold`` bases long, and removes reads from the stream
less than ``threshold`` bases long.
