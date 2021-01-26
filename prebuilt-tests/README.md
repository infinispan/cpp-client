# RPM test suite #
This folders contains code to run testsuite against prebuild artifacts (rpms, zip)
One subfolder for distro is provided, they all work in the same way:

1. Change to you target distro
2. Run `build-docker.sh`
3. Set the following envvars:
	-
	- SERVER_ZIP=Full name of the server zip file. (mandatory)
	- RPM_PATH=(*$HOME/git/cpp-client/prebuilt-tests/rpm/*). Path where you installed/unpacked the artifact, it should contain the `usr` folder.
	- TEST_SRC_PATH=(*$HOME/git/cpp-client/*). Path to the testsuite source code.

If are ok with the defaults you can just set the SERVER_ZIP var.
4. Run `run-docker.sh`

That's all
