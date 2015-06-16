# cpXmlCmd
A file copy program controlled by an xml document. The program can optionally copy files named on the command line (like linux *cp*).

**Command line syntax**:

	cpXmlCmd [-l outputdocument] FilesToCopy

*-l*
	The "-l" flag specifies that a summary log of the execution should be produced.	The "outputdocument" parameter gives the file name the output will be written to. The output will be xml regardless of the name of the file. See the  [cpXmlCmdLogSchema.xsd](https://github.com/jsprenkle/cpXmlCmd/blob/master/cpXmlCmdLogSchema.xsd) schema for specification of the file content.

*FilesToCopy*
This parameter names the xml file containing the list of copy operations to perform. See the [cpXmlCmdSchema.xsd](https://github.com/jsprenkle/cpXmlCmd/blob/master/cpXmlCmdSchema.xsd) schema for specification of the file content.

**Alternative usage**:

	cpXmlCmd [-l outputdocument] Source Destination

*Source*
This parameter names the source file containing to be copied.

*Destination*
This parameter names the destination file containing to be copied.

**Exit codes**:

The program returns a non zero exit code for a program failure. A failure indication is returned for incorrect usages. A file not being copied is not considered a "failure". The log file must be examined to determine if one or more files are not copied.

**Notes**:

If the destination directory of a file does not exist the program will create the directory automatically.

**Binary versions**:
[Windows executable](https://github.com/jsprenkle/cpXmlCmd/blob/master/dist/Release/MinGW-Windows/cpXmlCmd.exe)

Linux version coming soon

