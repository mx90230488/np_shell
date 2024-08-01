# NP_Shell
### Demo
```
bash$ ./npshell
% setenv PATH ./bin
% printenv PATH
./bin
% ls
bin  Makefile  npshell	npshell.cpp  README.md	test.html
% ls bin
cat  ls  noop  noop.cpp  number  number.cpp  removetag	removetag0 removetag0.cpp  removetag.cpp
% cat test.html > test1.txt
% cat test1.txt
<!test.html>
<TITLE>Test</TITLE>
<BODY>This is a <b>test</b> program
for ras.
</BODY>
% removetag test.html

Test
This is a test program
for ras.

% removetag test.html > test2.txt
% cat test2.txt

Test
This is a test program
for ras.

% removetag0 test.html
Error: illegal tag "!test.html"

Test
This is a test program
for ras.

% removetag0 test.html > test2.txt
Error: illegal tag "!test.html"
% cat test2.txt

Test
This is a test program
for ras.

% removetag test.html | number
   1
   2 Test
   3 This is a test program
   4 for ras.
   5
% removetag test.html |1
% number
   1
   2 Test
   3 This is a test program
   4 for ras.
   5
% removetag test.html |2
% ls
bin  Makefile  npshell	npshell.cpp  README.md	test1.txt  test2.txt  test.html
% number
   1
   2 Test
   3 This is a test program
   4 for ras.
   5
% removetag test.html |2
% removetag test.html |1
% number
   1
   2 Test
   3 This is a test program
   4 for ras.
   5
   6
   7 Test
   8 This is a test program
   9 for ras.
  10
% removetag test.html |2
% removetag test.html |1
% number |1
% number
   1    1
   2    2 Test
   3    3 This is a test program
   4    4 for ras.
   5    5
   6    6
   7    7 Test
   8    8 This is a test program
   9    9 for ras.
  10   10
% removetag test.html | number |1
% number
   1    1
   2    2 Test
   3    3 This is a test program
   4    4 for ras.
   5    5
% ls |2
% ls
bin  Makefile  npshell	npshell.cpp  README.md	test1.txt  test2.txt  test.html
% number > test3.txt
% cat test3.txt
   1 bin
   2 Makefile
   3 npshell
   4 npshell.cpp
   5 README.md
   6 test1.txt
   7 test2.txt
   8 test.html
% removetag0 test.html |1
Error: illegal tag "!test.html"
% number
   1
   2 Test
   3 This is a test program
   4 for ras.
   5
% removetag0 test.html !1
% number
   1 Error: illegal tag "!test.html"
   2
   3 Test
   4 This is a test program
   5 for ras.
   6
% exit
bash$ 
```