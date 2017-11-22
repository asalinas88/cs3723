#! /usr/bin/python

import sys
import re

# import functions here
from p5team import processTeams
from p5week import processWeeks
# check to make sure a file is being read in
if len(sys.argv) < 2:
    print("filename needed as command argument")
    sys.exit(1)
file = open(sys.argv[1], "r")
while True:
    inputLine = file.readline()     # this reads one text line from file
    if inputLine == "":
        break
    inputLine = inputLine.rstrip('\n')
    print(">>>", inputLine)   
    # run functions for team and week here
    if inputLine == "TEAMS":
        # grab team data here
        processTeams(file)
    # if inputline = week x
    weekMatch = re.compile(r'WEEK \d')
    matchObj = weekMatch.search(inputLine)
    if matchObj != None:
        processWeeks(file, matchObj.group(0))


file.close()

