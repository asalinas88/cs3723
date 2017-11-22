#! /usr/bin/python
import re
import pprint

# global dictionaries
teamNames = {}
collegeScore = {}

def printTeams(message):
    print(message)
    print("\tTeams Wins Losses")
    for team, score in collegeScore.items():
        print("\t%s\t%s    %s"%(team, collegeScore[team][0],collegeScore[team][1]))
def processTeams(team):
    for inputLine in team:
        inputLine = inputLine.rstrip('\n')
        if inputLine == "ENDTEAMS":
            printTeams('INITAL')
            break
        collegeName = inputLine.split(" ", 1)[0]
        teamName = inputLine.split(" ", 1)[1]
        teamName = teamName.split(", ")
        teamName.sort(reverse=True, key=len)
        teamNames[collegeName] = teamName
        collegeScore[collegeName] = [0,0]

   # pprint.pprint(teamNames)

