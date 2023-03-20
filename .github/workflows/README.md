# cpp-client actions overview

There are two workflows enabled.

## CD
This is the coninuous delivery workflow and it's triggered by a push of a tag with pattern "9.2.*" on the repo. It produces a release named as the tag

## CI
This is the coninuous integration workflow and it's triggered by a push 9.2.x branch and on each PR created against 9.2.x branch. I produces a release named 9.2.x-latest


