.. pyzfile documentation master file, created by
   sphinx-quickstart on Fri Oct 21 11:28:02 2022.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to pyzfile's documentation!
===================================

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   modules

Prerequisites
=============
IBM Python SDK for z/OS v3.10

Installation
============
Install from `PyPi`::

    pip install pyzfile

Building from source
====================
To build from source you must have Python 3.10 and the IBM xlclang++ compiler installed.::

    git clone git@github.com:daveyc/pyzfile.git
    cd pyzfile
    git submodule update --init
    pip install .

Examples
========
The Python SDK for z/OS uses enhanced ASCII so conversions to/from EBCDIC code pages are required to process text files
or string fields from binary files in the UTF-8 codeset.

Read a text file using BSAM
---------------------------
MVS text files will almost certainly be encoding in EBCDIC. Specify the ``encoding`` parameter to decode the EBCDIC
file to UTF-8.

.. code-block:: python

    from pyzfile import *
    try:
        with ZFile("//'USERID.CNTL(JCL)''", "rb,type=record",encoding='cp1047') as file:
            for rec in file:
                print(rec)
    except ZFileError as e:
        print(e)

Write to a text file using QSAM
-------------------------------
.. code-block:: python

    from pyzfile import *
    recs = ['record number one', 'record number two', 'record number three']
    try:
        with ZFile("//'USERID.CNTL(JCL)''", "wb,type=record,noseek", encoding='cp1047') as file:
            for rec in recs:
                file.write(rec)
    except ZFileError as e:
        print(e)

Read a binary file using QSAM
-----------------------------

.. code-block:: python

    from pyzfile import *
    try:
        with ZFile("//'SYSTEM.DAILY.SMF'", "rb,type=record,noseek") as file:
            for rec in file:
                pass
    except ZFileError as e:
        print(e)

Parse a YAML text file from a PDS member
----------------------------------------
.. note::
    This example uses a third party package for parsing YAML which can be installed using ``pip install ruamel.yaml``.

.. code-block:: python

    from pyzfile import *
    from ruamel.yaml import YAML

    yaml = YAML()

    try:
        with ZFile("//CNTL(YAML)", "rt", encoding='cp1047') as file:
            print(yaml.load(file.reads()))
    except ZFileError as e:
        print(e)

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
