%define name krename
%define version 3.0.14

Summary: A powerfull batch renamer for KDE
Name: %{name}
Version: %{version}
Release: 1
License: GPL
Vendor: Dominik Seichter <domseichter@web.de>
Url: http://krename.sourceforge.net
Packager: Dominik Seichter <domseichter@web.de>
Group: kde/utilities
Source: %{name}-%{version}.tar.bz2
BuildRoot: /var/tmp/%{name}-%{version}

%description
Krename is a very powerful batch file renamer for KDE3 which can rename a list of files based on a set of expressions. It can copy/move the files to another directory or simply rename the input files. Krename supports many conversion operations, including conversion of a filename to lowercase or to uppercase, conversion of the first letter of every word to uppercase, adding numbers to filenames, finding and replacing parts of the filename, and many more. It can also change access and modification dates, permissions, and file ownership.

%prep
%setup

%build
# Look for common rpm-options:
if [ -f /etc/opt/kde3/common_options ]; then
  . /etc/opt/kde3/common_options
  ./configure $configkde
else
  ./configure
fi

# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
 numprocs=1
fi

make -j$numprocs

%install
make install-strip DESTDIR=$RPM_BUILD_ROOT

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/master.list
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >>  $RPM_BUILD_DIR/master.list
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >>  $RPM_BUILD_DIR/master.list

%clean
cd $RPM_BUILD_DIR
rm -rf $RPM_BUILD_ROOT
rm -rf %{name}-%{version}
rm master.list

%files -f ../master.list
