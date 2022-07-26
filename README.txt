
		      IDF v2 to AP210 Converter

This is a bidirectional converter for transfering data between IDF
version 2 and AP210 TC1.

The converter is in two parts.  One part transfers the data between the
IDF v2 files and a custom "IDF-like" EXPRESS schema.  This part is
written in a conventional programming language (e.g. C, C++, Lisp).

The second part transfers the data between the IDF-like EXPRESS and
AP210 TC1.  This part is also broken down into two components.  One
component transfers the data between the IDF-like EXPRESS and the
AP210 TC1 ARM (Application Requirements Model).  The other component
transfers the data between the AP210 TC1 ARM and the AP210 TC1 AIM
(Application Interpreted Model).

This example converter has be produced as a project within the PDES,
Inc. consortium.  Contributions to and funding for this converter have
been provided by Delphi Delco, Rockwell Collins, The Boeing Company,
NASA, and Northrop Grumman.

This example converter is being provided primarily as an example of
how to build a large EXPRESS-X schema.
