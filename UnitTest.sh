#/bin/bash
#
# unit test for cpXmlCmd
# Author: Jay Sprenkle
#

# create a temp directory. Remove everything when the script completes
MYTMPDIR=tmp
mkdir $MYTMPDIR
trap "rm -rf $MYTMPDIR" EXIT

#
# where is the binary?
#
cpXmlCmd=dist/Release/MinGW-Windows/cpXmlCmd.exe

#
# nothing on command line, should fail
#
echo Begin negative parameters test
$cpXmlCmd 2>$MYTMPDIR/null
rc=$?
if [ "$rc" != "2" ]
  then
    exit 2
fi

#
# happy path
#
echo Begin happy path test
rm two.file 2>one.file

cat >$MYTMPDIR/FilesToCopy.xml <<xyzzy
<ns:cpXmlCmd xmlns:ns="http://www.XmlCommandLine.org/cpXmlCmd/1.0">
    <ns:Content>
        <ns:cp Source="one.file" Destination="two.file" />
    </ns:Content>
</ns:cpXmlCmd>
xyzzy
if [ ! -f $MYTMPDIR/FilesToCopy.xml ]
  then
    echo "FilesToCopy.xml" does not exist but should
    exit 1
fi
rm $MYTMPDIR/outputdocument.xml 2>one.file

$cpXmlCmd -l $MYTMPDIR/outputdocument.xml $MYTMPDIR/FilesToCopy.xml
rc=$?
if [ "$rc" != "0" ]
  then
    echo cpXmlCmd failed
    exit 1
fi
if [ ! -f $MYTMPDIR/outputdocument.xml ]
  then
    echo "outputdocument.xml" does not exist but should
    exit 1
fi

# compare to expected output
# ignore dates and environmental differences
cat >$MYTMPDIR/RemoveDates <<xyzzy
s/<Environment>.*<\/Environment>//g
s/="[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9]:[0-9][0-9]:[0-9][0-9]Z"//g
xyzzy
sed -f $MYTMPDIR/RemoveDates ExpectedOutput.xml >$MYTMPDIR/file1
sed -f $MYTMPDIR/RemoveDates $MYTMPDIR/outputdocument.xml >$MYTMPDIR/file2
diff $MYTMPDIR/file1 $MYTMPDIR/file2
rc=$?
if [ "$rc" != "0" ]
  then
    echo Output document does not match the expected output
    exit 1
fi
echo "  "Output document is correct

if [ ! -f one.file ]
  then
    echo "one.file" does not exist but should
    exit 1
fi
if [ ! -f two.file ]
  then
    echo "two.file" does not exist but should
    exit 1
fi
diff one.file two.file
rc=$?
if [ "$rc" != "0" ]
  then
    echo the copied file is not identical to the source
    exit 1
fi

echo "  "operation succeeded

rm two.file


#
# test directory creation on local file system
#
echo Begin directory creation test
rm $MYTMPDIR/x/y/z/two.file 2>one.file

cat >$MYTMPDIR/FilesToCopy.xml <<xyzzy2
<ns:cpXmlCmd xmlns:ns="http://www.XmlCommandLine.org/cpXmlCmd/1.0">
    <ns:Content>
        <ns:cp Source="one.file" Destination="$MYTMPDIR/x/y/z/two.file" />
    </ns:Content>
</ns:cpXmlCmd>
xyzzy2
if [ ! -f $MYTMPDIR/FilesToCopy.xml ]
  then
    echo "FilesToCopy.xml" does not exist but should
    exit 1
fi
rm $MYTMPDIR/outputdocument.xml 2>one.file

$cpXmlCmd -l $MYTMPDIR/outputdocument.xml $MYTMPDIR/FilesToCopy.xml
rc=$?
if [ "$rc" != "0" ]
  then
    echo cpXmlCmd failed
    exit 1
fi
if [ ! -f $MYTMPDIR/outputdocument.xml ]
  then
    echo "outputdocument.xml" does not exist but should
    exit 1
fi

if [ ! -f one.file ]
  then
    echo "one.file" does not exist but should
    exit 1
fi
if [ ! -f $MYTMPDIR/x/y/z/two.file ]
  then
    echo "x/y/z/two.file" does not exist but should
    exit 1
fi
diff one.file $MYTMPDIR/x/y/z/two.file
rc=$?
if [ "$rc" != "0" ]
  then
    echo the copied file is not identical to the source
    exit 1
fi

echo "  "operation succeeded

rm $MYTMPDIR/x/y/z/two.file

echo Tests completed
