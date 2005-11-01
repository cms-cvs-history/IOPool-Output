#!/bin/sh

rm -f *.root *.xml *.xml.BAK

cmsRun --parameter-set RefWriteTest.cfg
