***********************
Sequences and Map Types
***********************

The following sections describe the sequence and mapping types that are built into Zerynth.

.. _iterator_types:

Iterator Types
==============

Python supports a concept of iteration over containers.  This is implemented
using two distinct methods; these are used to allow user-defined classes to
support iteration.

One method needs to be defined for container objects to provide iteration
support:

.. method:: container.__iter__()

   Return an iterator object.  The object is required to support the iterator
   protocol described below.

The iterator objects themselves are required to support the following two
methods, which together form the :dfn:`iterator protocol`:


.. method:: iterator.__iter__()

   Return the iterator object itself.  This is required to allow both containers
   and iterators to be used with the :keyword:`for` and :keyword:`in` statements.


.. method:: iterator.__next__()

   Return the next item from the container.  If there are no further items, raise
   the :exc:`StopIteration` exception.  


Once an iterator's :meth:`~iterator.__next__` method raises
:exc:`StopIteration`, it must continue to do so on subsequent calls.
Implementations that do not obey this property are deemed broken.




.. _typesseq:

Sequence Types
==============

There are three basic sequence types: lists, tuples, and range objects.
Additional sequence types tailored for processing of
:ref:`binary data <binaryseq>` and :ref:`text strings <textseq>` are
described in dedicated sections.


Common Sequence Operations
--------------------------

The operations in the following table are supported by most sequence types,
both mutable and immutable. 

This table lists the sequence operations sorted in ascending priority.  In the
table, **s** and **t** are sequences of the same type, **n**, **i**, **j** and **k** are
integers and **x** is an arbitrary object that meets any type and value
restrictions imposed by **s**.

The ``in`` and ``not in`` operations have the same priorities as the
comparison operations. The ``+`` (concatenation) and ``*`` (repetition)
operations have the same priority as the corresponding numeric operations.

+--------------------------+--------------------------------+----------+
| Operation                | Result                         | Notes    |
+==========================+================================+==========+
| ``x in s``               | ``True`` if an item of **s** is| \(1)     |
|                          | equal to **x**, else ``False`` |          |
+--------------------------+--------------------------------+----------+
| ``x not in s``           | ``False`` if an item of **s**  | \(1)     |
|                          | is equal to **x**, else        |          |
|                          | ``True``                       |          |
+--------------------------+--------------------------------+----------+
| ``s + t``                | the concatenation of **s** and | (6)(7)   |
|                          | **t**                          |          |
+--------------------------+--------------------------------+----------+
| ``s * n`` or             | equivalent to adding **s** to  | (2)(7)   |
| ``n * s``                | itself **n** times             |          |
+--------------------------+--------------------------------+----------+
| ``s[i]``                 | **i**-th item of **s**, origin | \(3)     |
|                          | 0                              |          |
+--------------------------+--------------------------------+----------+
| ``s[i:j]``               | slice of **s** from **i**      | (3)(4)   |
|                          | to **j**                       |          |
+--------------------------+--------------------------------+----------+
| ``s[i:j:k]``             | slice of **s** from **i** to   | (3)(5)   |
|                          | **j** with step **k**          |          |
+--------------------------+--------------------------------+----------+
| ``len(s)``               | length of **s**                |          |
+--------------------------+--------------------------------+----------+
| ``min(s)``               | smallest item of **s**         |          |
+--------------------------+--------------------------------+----------+
| ``max(s)``               | largest item of **s**          |          |
+--------------------------+--------------------------------+----------+
| ``s.index(x[, i[, j]])`` | index of the first occurrence  | \(8)     |
|                          | of **x** in **s** (at or after |          |
|                          | **i** and before **j**)        |          |
+--------------------------+--------------------------------+----------+
| ``s.count(x)``           | total number of occurrences of |          |
|                          | **x** in **s**                 |          |
+--------------------------+--------------------------------+----------+

Sequences of the same type also support comparisons.  In particular, tuples
and lists are compared lexicographically by comparing corresponding elements.
This means that to compare equal, every element must compare equal and the
two sequences must be of the same type and have the same length.

Notes:

(1)
   While the ``in`` and ``not in`` operations are used only for simple
   containment testing in the general case, some specialised sequences
   (such as :class:`str`, :class:`bytes` and :class:`bytearray`) also use
   them for subsequence testing::

      >>> "gg" in "eggs"
      True

(2)
   Values of **n** less than ``0`` are treated as ``0`` (which yields an empty
   sequence of the same type as **s**).  Note that items in the sequence **s**
   are not copied; they are referenced multiple times.  This often haunts
   new Python programmers; consider::

      >>> lists = [[]] * 3
      >>> lists
      [[], [], []]
      >>> lists[0].append(3)
      >>> lists
      [[3], [3], [3]]

   What has happened is that ``[[]]`` is a one-element list containing an empty
   list, so all three elements of ``[[]] * 3`` are references to this single empty
   list.  Modifying any of the elements of ``lists`` modifies this single list.
   You can create a list of different lists this way::

      >>> lists = [[] for i in range(3)]
      >>> lists[0].append(3)
      >>> lists[1].append(5)
      >>> lists[2].append(7)
      >>> lists
      [[3], [5], [7]]

(3)
   If **i** or **j** is negative, the index is relative to the end of the string:
   ``len(s) + i`` or ``len(s) + j`` is substituted.  But note that ``-0`` is
   still ``0``.

(4)
   The slice of **s** from **i** to **j** is defined as the sequence of items with index
   **k** such that ``i <= k < j``.  If **i** or **j** is greater than ``len(s)``, use
   ``len(s)``.  If **i** is omitted or ``None``, use ``0``.  If **j** is omitted or
   ``None``, use ``len(s)``.  If **i** is greater than or equal to **j**, the slice is
   empty.

(5)
   The slice of **s** from **i** to **j** with step **k** is defined as the sequence of
   items with index  ``x = i + n*k`` such that ``0 <= n < (j-i)/k``.  In other words,
   the indices are ``i``, ``i+k``, ``i+2*k``, ``i+3*k`` and so on, stopping when
   **j** is reached (but never including **j**).  If **i** or **j** is greater than
   ``len(s)``, use ``len(s)``.  If **i** or **j** are omitted or ``None``, they become
   "end" values (which end depends on the sign of **k**).  Note, **k** cannot be zero.
   If **k** is ``None``, it is treated like ``1``.

(6)
   Concatenating immutable sequences always results in a new object.  This
   means that building up a sequence by repeated concatenation will have a
   quadratic runtime cost in the total sequence length.  To get a linear
   runtime cost, you must switch to one of the alternatives below:

   * if concatenating :class:`str` objects, you can build a list and use
     :meth:`str.join` at the end.

   * if concatenating :class:`bytes` objects, you can similarly use
     :meth:`bytes.join` or you can do in-place
     concatenation with a :class:`bytearray` object.  :class:`bytearray`
     objects are mutable and have an efficient overallocation mechanism

   * if concatenating :class:`tuple` objects, extend a :class:`list` instead

   * for other types, investigate the relevant class documentation


(7)
  Some sequence types (such as :class:`range`) only support item sequences
  that follow specific patterns, and hence don't support sequence
  concatenation or repetition.

(8)
   ``index`` raises :exc:`ValueError` when **x** is not found in **s**.
   When supported, the additional arguments to the index method allow
   efficient searching of subsections of the sequence. Passing the extra
   arguments is roughly equivalent to using ``s[i:j].index(x)``, only
   without copying any data and with the returned index being relative to
   the start of the sequence rather than the start of the slice.


Immutable Sequence Types
------------------------

The only feature that immutable sequence types generally implement that is
not also implemented by mutable sequence types is hashing support.

This support allows immutable sequences, such as :class:`tuple` instances, to
be used as :class:`dict` keys and stored in :class:`set` and :class:`frozenset`
instances.

Attempting to hash an immutable sequence that contains unhashable values will
result in :exc:`TypeError`.


Mutable Sequence Types
----------------------

The operations in the following table are defined on mutable sequence types.

In the table **s** is an instance of a mutable sequence type, **t** is any
iterable object and **x** is an arbitrary object that meets any type
and value restrictions imposed by **s** (for example, :class:`bytearray` only
accepts integers that meet the value restriction ``0 <= x <= 255``).


+------------------------------+--------------------------------+---------------------+
| Operation                    | Result                         | Notes               |
+==============================+================================+=====================+
| ``s[i] = x``                 | item **i** of **s** is replaced|                     |
|                              | by **x**                       |                     |
+------------------------------+--------------------------------+---------------------+
| ``s[i:j] = t``               | slice of **s** from **i** to   |                     |
|                              | **j** is replaced by the       |                     |
|                              | contents of the iterable **t** |                     |
+------------------------------+--------------------------------+---------------------+
| ``del s[i:j]``               | same as ``s[i:j] = []``        | NOT SUPPORTED YET   |
+------------------------------+--------------------------------+---------------------+
| ``s[i:j:k] = t``             | the elements of ``s[i:j:k]``   | \(1)                |
|                              | are replaced by those of **t** |                     |
+------------------------------+--------------------------------+---------------------+
| ``del s[i:j:k]``             | removes the elements of        | NOT SUPPORTED YET   |
|                              | ``s[i:j:k]`` from the list     |                     |
+------------------------------+--------------------------------+---------------------+
| ``s.append(x)``              | appends **x** to the end of the|                     |
|                              | sequence (same as              |                     |
|                              | ``s[len(s):len(s)] = [x]``)    |                     |
+------------------------------+--------------------------------+---------------------+
| ``s.clear()``                | removes all items from ``s``   | \(5)                |
|                              | (same as ``del s[:]``)         |                     |
+------------------------------+--------------------------------+---------------------+
| ``s.copy()``                 | creates a shallow copy of ``s``| \(5)                |
|                              | (same as ``s[:]``)             |                     |
+------------------------------+--------------------------------+---------------------+
| ``s.extend(t)`` or           | extends **s** with the         |                     |
| ``s += t``                   | contents of **t** (for the     |                     |
|                              | most part the same as          |                     |
|                              | ``s[len(s):len(s)] = t``)      |                     |
+------------------------------+--------------------------------+---------------------+
| ``s *= n``                   | updates **s** with its contents| \(6)                |
|                              | repeated **n** times           |                     |
+------------------------------+--------------------------------+---------------------+
| ``s.insert(i, x)``           | inserts **x** into **s** at the|                     |
|                              | index given by **i**           |                     |
|                              | (same as ``s[i:i] = [x]``)     |                     |
+------------------------------+--------------------------------+---------------------+
| ``s.pop([i])``               | retrieves the item at **i** and| \(2)                |
|                              | also removes it from **s**     |                     |
+------------------------------+--------------------------------+---------------------+
| ``s.remove(x)``              | remove the first item from     | \(3)                |
|                              | **s** where ``s[i] == x``      |                     |
+------------------------------+--------------------------------+---------------------+
| ``s.reverse()``              | reverses the items of **s** in | \(4)                |
|                              | place                          |                     |
+------------------------------+--------------------------------+---------------------+


Notes:

(1)
   **t** must have the same length as the slice it is replacing.

(2)
   The optional argument **i** defaults to ``-1``, so that by default the last
   item is removed and returned.

(3)
   ``remove`` raises :exc:`ValueError` when **x** is not found in **s**.

(4)
   The :meth:`reverse` method modifies the sequence in place for economy of
   space when reversing a large sequence.  To remind users that it operates by
   side effect, it does not return the reversed sequence.

(5)
   :meth:`clear` and :meth:`!copy` are included for consistency with the
   interfaces of mutable containers that don't support slicing operations
   (such as :class:`dict` and :class:`set`)

(6)
   The value **n** is an integer. Zero and negative values of **n** clear
   the sequence.  Items in the sequence are not copied; they are referenced
   multiple times, as explained for ``s * n``.


.. _typesseq-list:

Lists
-----

Lists are mutable sequences, typically used to store collections of
homogeneous items (where the precise degree of similarity will vary by
application).


Lists may be constructed in several ways:

* Using a pair of square brackets to denote the empty list: ``[]``
* Using square brackets, separating items with commas: ``[a]``, ``[a, b, c]``
* Using a list comprehension: ``[x for x in iterable]``

Lists implement all of the :ref:`common <typesseq-common>` and
:ref:`mutable <typesseq-mutable>` sequence operations. 


.. _typesseq-tuple:

Tuples
------

Tuples are immutable sequences, typically used to store collections of
heterogeneous data (such as the 2-tuples produced by the :func:`enumerate`
built-in). Tuples are also used for cases where an immutable sequence of
homogeneous data is needed.


Tuples may be constructed in a number of ways:

* Using a pair of parentheses to denote the empty tuple: ``()``
* Using a trailing comma for a singleton tuple: ``a,`` or ``(a,)``
* Separating items with commas: ``a, b, c`` or ``(a, b, c)``

Tuples implement all of the :ref:`common <typesseq-common>` sequence
operations.


.. _typesseq-range:

Ranges
------

The :class:`range` type represents an immutable sequence of numbers and is
commonly used for looping a specific number of times in :keyword:`for`
loops.

.. class:: range(stop)
           range(start, stop[, step])

   The arguments to the range constructor must be integers.
   If the *step* argument is omitted, it defaults to ``1``.
   If the *start* argument is omitted, it defaults to ``0``.
   If *step* is zero, :exc:`ValueError` is raised.

   For a positive *step*, the contents of a range ``r`` are determined by the
   formula ``r[i] = start + step*i`` where ``i >= 0`` and
   ``r[i] < stop``.

   For a negative *step*, the contents of the range are still determined by
   the formula ``r[i] = start + step*i``, but the constraints are ``i >= 0``
   and ``r[i] > stop``.

   A range object will be empty if ``r[0]`` does not meet the value
   constraint. Ranges do support negative indices, but these are interpreted
   as indexing from the end of the sequence determined by the positive
   indices.

   Ranges implement all of the :ref:`common <typesseq-common>` sequence operations
   except concatenation and repetition (due to the fact that range objects can
   only represent sequences that follow a strict pattern and repetition and
   concatenation will usually violate that pattern).


The advantage of the :class:`range` type over a regular :class:`list` or
:class:`tuple` is that a :class:`range` object will always take the same
(small) amount of memory, no matter the size of the range it represents (as it
only stores the ``start``, ``stop`` and ``step`` values, calculating individual
items and subranges as needed).


Testing range objects for equality with ``==`` and ``!=`` compares
them as sequences.  That is, two range objects are considered equal if
they represent the same sequence of values.  (Note that two range
objects that compare equal might have different :attr:`~range.start`,
:attr:`~range.stop` and :attr:`~range.step` attributes, for example
``range(0) == range(2, 1, 3)`` or ``range(0, 3, 2) == range(0, 4, 2)``.)


.. _textseq:

Strings
=======

Textual data in Python is handled with :dfn:`strings`.
Strings are immutable :ref:`sequences <typesseq>` of 8 bit characters. Zerynth does not support Unicode yet.
String literals are written in a variety of ways:

* Single quotes: ``'allows embedded "double" quotes'``
* Double quotes: ``"allows embedded 'single' quotes"``.
* Triple quoted: ``'''Three single quotes'''``, ``"""Three double quotes"""``

Triple quoted strings may span multiple lines - all associated whitespace will
be included in the string literal.

String literals that are part of a single expression and have only whitespace
between them will be implicitly converted to a single string literal. That
is, ``("spam " "eggs") == "spam eggs"``.


Strings may also be created from other objects using the :class:`str`
builtin.

Since there is no separate "character" type, indexing a string produces
strings of length 1. That is, for a non-empty string **s**, ``s[0] == s[0:1]``.

There is also no mutable string type, but :meth:`str.join` can be used to efficiently construct strings from
multiple fragments.

.. class:: str(object='')

   Return a :ref:`string <textseq>` version of *object*.  If *object* is not
   provided, returns the empty string. Returns
   :meth:`object.__str__() <object.__str__>`, which is the "informal" or nicely
   printable string representation of *object*.  For string objects, this is
   the string itself.  If *object* does not have a :meth:`~object.__str__`
   method, then :func:`str` raises TypeError.


.. _string-methods:

String Methods
--------------

Strings implement all of the :ref:`common <typesseq-common>` sequence
operations, along with the additional methods described below.

Strings also support string formatting based on C ``printf`` style formatting..


.. method:: str.count(sub[, start[, end]])

   Return the number of non-overlapping occurrences of substring *sub* in the
   range [*start*, *end*].  Optional arguments *start* and *end* are
   interpreted as in slice notation.


.. method:: str.endswith(suffix[, start[, end]])

   Return ``True`` if the string ends with the specified *suffix*, otherwise return
   ``False``.  With optional
   *start*, test beginning at that position.  With optional *end*, stop comparing
   at that position.


.. method:: str.find(sub[, start[, end]])

   Return the lowest index in the string where substring *sub* is found within
   the slice ``s[start:end]``.  Optional arguments *start* and *end* are
   interpreted as in slice notation.  Return ``-1`` if *sub* is not found.

   .. note::

      The :meth:`~str.find` method should be used only if you need to know the
      position of *sub*.  To check if *sub* is a substring or not, use the
      :keyword:`in` operator.


.. method:: str.index(sub[, start[, end]])

   Like :meth:`~str.find`, but raise :exc:`ValueError` when the substring is
   not found.


.. method:: str.join(iterable)

   Return a string which is the concatenation of the strings in the
   :term:`iterable` *iterable*. *iterable* must be a builtin sequence or builtin map. 
   A :exc:`TypeError` will be raised if there are
   any non-string values in *iterable*, excluding :class:`bytes` and :class:`byterray` objects that are treated as strings.  The separator between elements is the string providing this method.


.. method:: str.lower()

   Return a copy of the string with all the cased characters converted to
   lowercase.


.. method:: str.replace(old, new)

   Return a copy of the string with all occurrences of substring *old* replaced by
   *new*.


.. method:: str.split(sep=None, maxsplit=-1)

   Return a list of the words in the string, using *sep* as the delimiter
   string.  If *maxsplit* is given, at most *maxsplit* splits are done (thus,
   the list will have at most ``maxsplit+1`` elements).  If *maxsplit* is not
   specified or ``-1``, then there is no limit on the number of splits
   (all possible splits are made).

   If *sep* is given, consecutive delimiters are not grouped together and are
   deemed to delimit empty strings (for example, ``'1,,2'.split(',')`` returns
   ``['1', '', '2']``).  The *sep* argument may consist of multiple characters
   (for example, ``'1<>2<>3'.split('<>')`` returns ``['1', '2', '3']``).
   Splitting an empty string with a specified separator returns ``['']``.

   For example::

      >>> '1,2,3'.split(',')
      ['1', '2', '3']
      >>> '1,2,3'.split(',', maxsplit=1)
      ['1', '2,3']
      >>> '1,2,,3,'.split(',')
      ['1', '2', '', '3', '']

   If *sep* is not specified, a different splitting algorithm is
   applied: runs of consecutive whitespace are regarded as a single separator,
   and the result will contain no empty strings at the start or end if the
   string has leading or trailing whitespace.  Consequently, splitting an empty
   string or a string consisting of just whitespace with a ``None`` separator
   returns ``[]``.

   For example::

      >>> '1 2 3'.split()
      ['1', '2', '3']
      >>> '1 2 3'.split(maxsplit=1)
      ['1', '2 3']
      >>> '   1   2   3   '.split()
      ['1', '2', '3']


.. method:: str.startswith(prefix[, start[, end]])

   Return ``True`` if string starts with the *prefix*, otherwise return ``False``.
   With optional *start*,
   test string beginning at that position.  With optional *end*, stop comparing
   string at that position.


.. method:: str.strip([chars [, dir=0]])

   Return a copy of the string with the leading and trailing characters removed.
   The *chars* argument is a string specifying the set of characters to be removed.
   If omitted, the *chars* argument defaults to removing whitespace.
   The *chars* argument is not a prefix or suffix; rather, all combinations of its
   values are stripped::

      >>> '   spacious   '.strip()
      'spacious'
      >>> 'www.example.com'.strip('cmowz.')
      'example'

   The *dir* parameter controls the side of stripping:

      * dir=0, strip *chars* from left and right side of *str*
      * dir>0, strip *chars* from left side of *str*
      * dir<0, strip *chars* from right side of *str*


.. method:: str.upper()

   Return a copy of the string with all the cased characters converted to
   uppercase.  



.. _old-string-formatting:

``printf``-style String Formatting
----------------------------------


String objects have one unique built-in operation: the ``%`` operator (modulo).
This is also known as the string *formatting* or *interpolation* operator.
Given ``format % values`` (where *format* is a string), ``%`` conversion
specifications in *format* are replaced with zero or more elements of *values*.
The effect is similar to using the :c:func:`sprintf` in the C language.

If *format* requires a single argument, *values* may be a single non-tuple
object. Otherwise, *values* must be a tuple or list with exactly the number of
items specified by the format string, or a single dictionary.

A conversion specifier contains two or more characters and has the following
components, which must occur in this order:

#. The ``'%'`` character, which marks the start of the specifier.

#. Mapping key (optional), consisting of a parenthesised sequence of characters
   (for example, ``(somename)``).

#. Conversion flags (optional), which affect the result of some conversion
   types.

#. Minimum field width (optional).  If specified as an ``'*'`` (asterisk), the
   actual width is read from the next element of the tuple in *values*, and the
   object to convert comes after the minimum field width and optional precision.

#. Precision (optional), given as a ``'.'`` (dot) followed by the precision.  If
   specified as ``'*'`` (an asterisk), the actual precision is read from the next
   element of the tuple in *values*, and the value to convert comes after the
   precision.

#. Conversion type.

When the right argument is a dictionary (or other mapping type), then the
formats in the string *must* include a parenthesised mapping key into that
dictionary inserted immediately after the ``'%'`` character. The mapping key
selects the value to be formatted from the mapping.  For example:

   >>> print('%(language)s has %(number)03d quote types.' %
   ...       {'language': "Python", "number": 2})
   Python has 002 quote types.

In this case no ``*`` specifiers may occur in a format (since they require a
sequential parameter list).

The conversion flag characters are:

+---------+---------------------------------------------------------------------+
| Flag    | Meaning                                                             |
+=========+=====================================================================+
| ``'0'`` | The conversion will be zero padded for numeric values.              |
+---------+---------------------------------------------------------------------+
| ``'-'`` | The converted value is left adjusted (overrides the ``'0'``         |
|         | conversion if both are given).                                      |
+---------+---------------------------------------------------------------------+
| ``' '`` | (a space) A blank should be left before a positive number (or empty |
|         | string) produced by a signed conversion.                            |
+---------+---------------------------------------------------------------------+
| ``'+'`` | A sign character (``'+'`` or ``'-'``) will precede the conversion   |
|         | (overrides a "space" flag).                                         |
+---------+---------------------------------------------------------------------+

The conversion types are \(1):

+------------+-----------------------------------------------------+-------+
| Conversion | Meaning                                             | Notes |
+============+=====================================================+=======+
| ``'d'``    | Signed integer decimal.                             |       |
+------------+-----------------------------------------------------+-------+
| ``'i'``    | Signed integer decimal.                             |       |
+------------+-----------------------------------------------------+-------+
| ``'u'``    | Obsolete type -- it is identical to ``'d'``.        |       |
+------------+-----------------------------------------------------+-------+
| ``'x'``    | Signed hexadecimal (lowercase).                     | \(2)  |
+------------+-----------------------------------------------------+-------+
| ``'X'``    | Signed hexadecimal (uppercase).                     | \(2)  |
+------------+-----------------------------------------------------+-------+
| ``'e'``    | Floating point exponential format (lowercase).      | \(3)  |
+------------+-----------------------------------------------------+-------+
| ``'E'``    | Floating point exponential format (uppercase).      | \(3)  |
+------------+-----------------------------------------------------+-------+
| ``'f'``    | Floating point decimal format.                      | \(3)  |
+------------+-----------------------------------------------------+-------+
| ``'F'``    | Floating point decimal format.                      | \(3)  |
+------------+-----------------------------------------------------+-------+
| ``'g'``    | Floating point format. Uses lowercase exponential   | \(3)  |
|            | format if exponent is less than -4 or not less than |       |
|            | precision, decimal format otherwise.                |       |
+------------+-----------------------------------------------------+-------+
| ``'G'``    | Floating point format. Uses uppercase exponential   | \(3)  |
|            | format if exponent is less than -4 or not less than |       |
|            | precision, decimal format otherwise.                |       |
+------------+-----------------------------------------------------+-------+
| ``'s'``    | String (converts any Python object using            | \(4)  |
|            | :func:`str`).                                       |       |
+------------+-----------------------------------------------------+-------+
| ``'%'``    | No argument is converted, results in a ``'%'``      |       |
|            | character in the result.                            |       |
+------------+-----------------------------------------------------+-------+

Notes:

(1)
   Not all Python conversion types and conversion flags are supported by Zerynth. 
   Refer to Python documentation for the (few) differences.

(2)
   Hexadecimal conversion does not produce the "0x" prefix.

(3)
   In Zerynth "g" and "e" modifiers, together with their uppercase versions are equivalent to "f" or "F".

   The precision determines the number of digits after the decimal point and
   defaults to 6.


(4)
   If precision is ``N``, the output is truncated to ``N`` characters.



.. _binaryseq:

Binary Sequence Types
=====================


The core built-in types for manipulating binary data are :class:`bytes` and
:class:`bytearray`. 

.. _typebytes:

Bytes
-----

Bytes objects are immutable sequences of single bytes. Bytes are very similar to strings in Zerynth, 
except that when iterating over them, integers are generated instead of strings.

Firstly, the syntax for bytes literals is largely the same as that for string
literals, except that a ``b`` prefix is added:

* Single quotes: ``b'still allows embedded "double" quotes'``
* Double quotes: ``b"still allows embedded 'single' quotes"``.
* Triple quoted: ``b'''3 single quotes'''``, ``b"""3 double quotes"""``

Only ASCII characters are permitted in bytes literals. Any binary values over 127 must be entered
into bytes literals using the appropriate escape sequence.

While bytes literals and representations are based on ASCII text, bytes
objects actually behave like immutable sequences of integers, with each
value in the sequence restricted such that ``0 <= x < 256`` (attempts to
violate this restriction will trigger :exc:`ValueError`). This is done
deliberately to emphasise that while many binary formats include ASCII based
elements and can be usefully manipulated with some text-oriented algorithms,
this is not generally the case for arbitrary binary data (blindly applying
text processing algorithms to binary data formats that are not ASCII
compatible will usually lead to data corruption).

In addition to the literal forms, bytes objects can be created in a number of
other ways:

* A zero-filled bytes object of a specified length: ``bytes(10)``
* From an iterable of integers: ``bytes([1,2,3])``

Since 2 hexadecimal digits correspond precisely to a single byte, hexadecimal
numbers are a commonly used format for describing binary data. 

Since bytes objects are sequences of integers (akin to a tuple), for a bytes
object *b*, ``b[0]`` will be an integer, while ``b[0:1]`` will be a bytes
object of length 1.  (This contrasts with text strings, where both indexing
and slicing will produce a string of length 1)


.. _typebytearray:

Bytearray Objects
-----------------

:class:`bytearray` objects are a mutable counterpart to :class:`bytes`
objects. There is no dedicated literal syntax for bytearray objects, instead
they are always created by calling the constructor:

* Creating an empty instance: ``bytearray()``
* Creating a zero-filled instance with a given length: ``bytearray(10)``
* From an iterable of integers: ``bytearray([1,2,3])``

As bytearray objects are mutable, they support the
:ref:`mutable <typesseq-mutable>` sequence operations in addition to the
common bytes and bytearray operations described in :ref:`bytes-methods`.


Since 2 hexadecimal digits correspond precisely to a single byte, hexadecimal
numbers are a commonly used format for describing binary data. 

Since bytearray objects are sequences of integers (akin to a list), for a
bytearray object *b*, ``b[0]`` will be an integer, while ``b[0:1]`` will be
a bytearray object of length 1.  (This contrasts with text strings, where
both indexing and slicing will produce a string of length 1)



.. _bytes-methods:

Bytes and Bytearray Operations
------------------------------


Both bytes and bytearray objects support the :ref:`common <typesseq-common>`
sequence operations. They interoperate not just with operands of the same
type, but with any :term:`bytes-like object`. Due to this flexibility, they can be
freely mixed in operations without causing errors. However, the return type
of the result may depend on the order of operands.

.. note::

   Contrary to Python, in Zerynth the methods on bytes and bytearray objects accept strings as their
   arguments, just as the methods on strings accept bytes as their
   arguments.  For example, you can write::

      a = "abc"
      b = a.replace(b"a", "f")

   and::

      a = b"abc"
      b = a.replace("a", b"f")


.. method:: bytes.count(sub[, start[, end]])
            bytearray.count(sub[, start[, end]])

   Return the number of non-overlapping occurrences of subsequence *sub* in
   the range [*start*, *end*].  Optional arguments *start* and *end* are
   interpreted as in slice notation.

   The subsequence to search for may be any :term:`bytes-like object` or an
   integer in the range 0 to 255.


.. method:: bytes.endswith(suffix[, start[, end]])
            bytearray.endswith(suffix[, start[, end]])

   Return ``True`` if the binary data ends with the specified *suffix*,
   otherwise return ``False``. With optional *start*, test beginning at that position.  With
   optional *end*, stop comparing at that position.


.. method:: bytes.find(sub[, start[, end]])
            bytearray.find(sub[, start[, end]])

   Return the lowest index in the data where the subsequence *sub* is found,
   such that *sub* is contained in the slice ``s[start:end]``.  Optional
   arguments *start* and *end* are interpreted as in slice notation.  Return
   ``-1`` if *sub* is not found.

   The subsequence to search for may be any :term:`bytes-like object` or an
   integer in the range 0 to 255.

   .. note::

      The :meth:`~bytes.find` method should be used only if you need to know the
      position of *sub*.  To check if *sub* is a substring or not, use the
      :keyword:`in` operator.

.. method:: bytes.index(sub[, start[, end]])
            bytearray.index(sub[, start[, end]])

   Like :meth:`~bytes.find`, but raise :exc:`ValueError` when the
   subsequence is not found.

   The subsequence to search for may be any :term:`bytes-like object` or an
   integer in the range 0 to 255.


.. method:: bytes.join(iterable)
            bytearray.join(iterable)

   Return a bytes or bytearray object which is the concatenation of the
   binary data sequences in the :term:`iterable` *iterable*.  A
   :exc:`TypeError` will be raised if there are any values in *iterable*
   that are not :term:`bytes-like objects <bytes-like object>` or strings.  
   The separator between elements is the contents
   of the bytes or bytearray object providing this method.


.. method:: bytes.replace(old, new)
            bytearray.replace(old, new)

   Return a copy of the sequence with all occurrences of subsequence *old*
   replaced by *new*.  

   .. note::

      The bytearray version of this method does *not* operate in place - it
      always produces a new object, even if no changes were made.



.. method:: bytes.startswith(prefix[, start[, end]])
            bytearray.startswith(prefix[, start[, end]])

   Return ``True`` if the binary data starts with the specified *prefix*,
   otherwise return ``False``. With optional *start*, test beginning at that position.
   With optional *end*, stop comparing at that position.



.. method:: bytes.split(sep=None, maxsplit=-1)
            bytearray.split(sep=None, maxsplit=-1)

   Split the binary sequence into subsequences of the same type, using *sep*
   as the delimiter string. If *maxsplit* is given and non-negative, at most
   *maxsplit* splits are done (thus, the list will have at most ``maxsplit+1``
   elements).  If *maxsplit* is not specified or is ``-1``, then there is no
   limit on the number of splits (all possible splits are made).

   If *sep* is given, consecutive delimiters are not grouped together and are
   deemed to delimit empty subsequences (for example, ``b'1,,2'.split(b',')``
   returns ``[b'1', b'', b'2']``).  The *sep* argument may consist of a
   multibyte sequence (for example, ``b'1<>2<>3'.split(b'<>')`` returns
   ``[b'1', b'2', b'3']``). Splitting an empty sequence with a specified
   separator returns ``[b'']`` or ``[bytearray(b'')]`` depending on the type
   of object being split.  

   For example::

      >>> b'1,2,3'.split(b',')
      [b'1', b'2', b'3']
      >>> b'1,2,3'.split(b',', maxsplit=1)
      [b'1', b'2,3']
      >>> b'1,2,,3,'.split(b',')
      [b'1', b'2', b'', b'3', b'']

   If *sep* is not specified, a different splitting algorithm
   is applied: runs of consecutive ASCII whitespace are regarded as a single
   separator, and the result will contain no empty strings at the start or
   end if the sequence has leading or trailing whitespace.  Consequently,
   splitting an empty sequence or a sequence consisting solely of ASCII
   whitespace without a specified separator returns ``[]``.

   For example::


      >>> b'1 2 3'.split()
      [b'1', b'2', b'3']
      >>> b'1 2 3'.split(maxsplit=1)
      [b'1', b'2 3']
      >>> b'   1   2   3   '.split()
      [b'1', b'2', b'3']


.. method:: bytes.strip([chars [,dir=0]])
            bytearray.strip([chars[,dir=0]])

   Return a copy of the binary sequence with the leading and trailing characters removed.
   The *chars* argument is a string or binary sequence specifying the set of characters to be removed.
   If omitted, the *chars* argument defaults to removing whitespace.
   The *chars* argument is not a prefix or suffix; rather, all combinations of its
   values are stripped::

      >>> b'   spacious   '.strip()
      b'spacious'
      >>> b'www.example.com'.strip(b'cmowz.')
      b'example'

   The *dir* parameter controls the side of stripping:

      * dir=0, strip *chars* from left and right side of *str*
      * dir>0, strip *chars* from left side of *str*
      * dir<0, strip *chars* from right side of *str*



The following methods on bytes and bytearray objects assume the use of ASCII
compatible binary formats and should not be applied to arbitrary binary data.
Note that all of the bytearray methods in this section do *not* operate in
place, and instead produce new objects.

.. method:: bytes.lower()
            bytearray.lower()

   Return a copy of the sequence with all the uppercase ASCII characters
   converted to their corresponding lowercase counterpart.

   Lowercase ASCII characters are those byte values in the sequence
   ``b'abcdefghijklmnopqrstuvwxyz'``. Uppercase ASCII characters
   are those byte values in the sequence ``b'ABCDEFGHIJKLMNOPQRSTUVWXYZ'``.

.. method:: bytes.upper()
            bytearray.upper()

   Return a copy of the sequence with all the lowercase ASCII characters
   converted to their corresponding uppercase counterpart.

   Lowercase ASCII characters are those byte values in the sequence
   ``b'abcdefghijklmnopqrstuvwxyz'``. Uppercase ASCII characters
   are those byte values in the sequence ``b'ABCDEFGHIJKLMNOPQRSTUVWXYZ'``.


Shorts and Shortarrays
----------------------

Both Shorts and Shortarrays are sequence types added by Zerynth to the standard Python. Shorts and shortsarray
objects actually behave like sequences of integers, with each
value in the sequence restricted such that ``0 <= x < 65536`` (attempts to
violate this restriction will trigger :exc:`ValueError`).

Shorts objects are immutable sequences of 16 bits integers. 
Shorts and shortsarray can't be specified with literals, but only using one of the following ways:

* A zero-filled bytes object of a specified length: ``shorts(10)``
* From an iterable of integers: ``shorts([1,2,3])``

Since 4 hexadecimal digits correspond precisely to a 16 bit integer, hexadecimal
numbers are a commonly used format for describing binary data. 

Since shorts objects are sequences of integers (akin to a tuple), for a shorts
object *b*, ``b[0]`` will be an integer, while ``b[0:1]`` will be a shorts
object of length 1. 



Shortarray objects are mutable sequences of 16 bits integers. Shorts and shortsarray can't be specified with literals, but only using one of the following ways:

* A zero-filled bytes object of a specified length: ``shortarray(10)``
* From an iterable of integers: ``shortarray([1,2,3])``

Since 4 hexadecimal digits correspond precisely to a 16 bit integer, hexadecimal
numbers are a commonly used format for describing binary data. 

Since shortarray objects are sequences of integers (akin to a list), for a shortarray
object *b*, ``b[0]`` will be an integer, while ``b[0:1]`` will be a shortarray
object of length 1. 

Shorts and shortarryas support only a limited set of methods: 

   * ``count`` and ``index`` with the same syntax and semantics of the corresponding methods for bytes
   * common methods for mutable objects (only for shortarrays).


.. _types-set:

Set Types
=========


A :dfn:`set` object is an unordered collection of distinct :term:`hashable` objects.
Common uses include membership testing, removing duplicates from a sequence, and
computing mathematical operations such as intersection, union, difference, and
symmetric difference.

Like other collections, sets support ``x in set``, ``len(set)``, and ``for x in
set``.  Being an unordered collection, sets do not record element position or
order of insertion.  Accordingly, sets do not support indexing, slicing, or
other sequence-like behavior.

There are currently two built-in set types, :class:`set` and :class:`frozenset`.
The :class:`set` type is mutable --- the contents can be changed using methods
like :meth:`~set.add` and :meth:`~set.remove`.  Since it is mutable, it has no
hash value and cannot be used as either a dictionary key or as an element of
another set.  The :class:`frozenset` type is immutable and :term:`hashable` ---
its contents cannot be altered after it is created; it can therefore be used as
a dictionary key or as an element of another set.

Non-empty sets (not frozensets) can be created by placing a comma-separated list
of elements within braces, for example: ``{'jack', 'sjoerd'}``, in addition to the
:class:`set` constructor.

The constructors for both classes work the same:

.. class:: set([iterable])
           frozenset([iterable])

   Return a new set or frozenset object whose elements are taken from
   *iterable*.  The elements of a set must be :term:`hashable`.  To
   represent sets of sets, the inner sets must be :class:`frozenset`
   objects.  If *iterable* is not specified, a new empty set is
   returned.

   Instances of :class:`set` and :class:`frozenset` provide the following
   operations:

   .. describe:: len(s)

      Return the cardinality of set **s**.

   .. describe:: x in s

      Test **x** for membership in **s**.

   .. describe:: x not in s

      Test **x** for non-membership in **s**.

   .. method:: isdisjoint(other)

      Return ``True`` if the set has no elements in common with *other*.  Sets are
      disjoint if and only if their intersection is the empty set.

   .. method:: issubset(other)
               set <= other

      Test whether every element in the set is in *other*.

   .. method:: set < other

      Test whether the set is a proper subset of *other*, that is,
      ``set <= other and set != other``.

   .. method:: issuperset(other)
               set >= other

      Test whether every element in *other* is in the set.

   .. method:: set > other

      Test whether the set is a proper superset of *other*, that is, ``set >=
      other and set != other``.

   .. method:: union(other, ...)
               set | other | ...

      Return a new set with elements from the set and all others.

   .. method:: intersection(other, ...)
               set & other & ...

      Return a new set with elements common to the set and all others.

   .. method:: difference(other, ...)
               set - other - ...

      Return a new set with elements in the set that are not in the others.

   .. method:: symmetric_difference(other)
               set ^ other

      Return a new set with elements in either the set or *other* but not both.

   .. method:: copy()

      Return a new set with a shallow copy of **s**.


   The following table lists operations available for :class:`set` that do not
   apply to immutable instances of :class:`frozenset`:

   .. method:: update(other, ...)
               set |= other | ...

      Update the set, adding elements from all others.

   .. method:: intersection_update(other, ...)
               set &= other & ...

      Update the set, keeping only elements found in it and all others.

   .. method:: difference_update(other, ...)
               set -= other | ...

      Update the set, removing elements found in others.

   .. method:: symmetric_difference_update(other)
               set ^= other

      Update the set, keeping only elements found in either set, but not in both.

   .. method:: add(elem)

      Add element *elem* to the set.

   .. method:: remove(elem)

      Remove element *elem* from the set.  Raises :exc:`KeyError` if *elem* is
      not contained in the set.

   .. method:: discard(elem)

      Remove element *elem* from the set if it is present.

   .. method:: pop()

      Remove and return an arbitrary element from the set.  Raises
      :exc:`KeyError` if the set is empty.

   .. method:: clear()

      Remove all elements from the set.



.. _typesmapping:

Mapping Types
=============

A :term:`mapping` object maps :term:`hashable` values to arbitrary objects.
Mappings are mutable objects.  There is currently only one standard mapping
type, the :dfn:`dictionary`.

A dictionary's keys are *almost* arbitrary values.  Values that are not
:term:`hashable`, that is, values containing lists, dictionaries or other
mutable types (that are compared by value rather than by object identity) may
not be used as keys.  Numeric types used for keys obey the normal rules for
numeric comparison: if two numbers compare equal (such as ``1`` and ``1.0``)
then they can be used interchangeably to index the same dictionary entry.  (Note
however, that since computers store floating-point numbers as approximations it
is usually unwise to use them as dictionary keys.)

Dictionaries can be created by placing a comma-separated list of ``key: value``
pairs within braces, for example: ``{'jack': 4098, 'sjoerd': 4127}`` or ``{4098:
'jack', 4127: 'sjoerd'}``, or by the :class:`dict` constructor.

.. class:: dict(*args)

   If no positional argument is given, an empty dictionary is created.
   If a positional argument is given and it is a mapping object, a dictionary
   is created with the same key-value pairs as the mapping object.  Otherwise,
   the positional argument must be an :term:`iterable` object.  Each item in
   the iterable must itself be an iterable with exactly two objects.  The
   first object of each item becomes a key in the new dictionary, and the
   second object the corresponding value.  If a key occurs more than once, the
   last value for that key becomes the corresponding value in the new
   dictionary.



   These are the operations that dictionaries support:

   .. describe:: len(d)

      Return the number of items in the dictionary *d*.

   .. describe:: d[key]

      Return the item of *d* with key *key*.  Raises a :exc:`KeyError` if *key* is
      not in the map.

   .. describe:: d[key] = value

      Set ``d[key]`` to *value*.

   .. describe:: del d[key]

      Remove ``d[key]`` from *d*.  Raises a :exc:`KeyError` if *key* is not in the
      map.

   .. describe:: key in d

      Return ``True`` if *d* has a key *key*, else ``False``.

   .. describe:: key not in d

      Equivalent to ``not key in d``.

   .. method:: clear()

      Remove all items from the dictionary.

   .. method:: copy()

      Return a shallow copy of the dictionary.

   .. method:: items()

      Return a new view of the dictionary's items (``(key, value)`` pairs).

   .. method:: keys()

      Return a new view of the dictionary's keys.  See the :ref:`documentation
      of view objects <dict-views>`.

   .. method:: pop(key[, default])

      If *key* is in the dictionary, remove it and return its value, else return
      *default*.  If *default* is not given and *key* is not in the dictionary,
      a :exc:`KeyError` is raised.

   .. method:: popitem()

      Remove and return an arbitrary ``(key, value)`` pair from the dictionary.

      :meth:`popitem` is useful to destructively iterate over a dictionary, as
      often used in set algorithms.  If the dictionary is empty, calling
      :meth:`popitem` raises a :exc:`KeyError`.

   .. method:: update([other])

      Update the dictionary with the key/value pairs from *other*, overwriting
      existing keys.  Return ``None``.

      :meth:`update` accepts another dictionary object.

   .. method:: values()

      Return a new view of the dictionary's values.



