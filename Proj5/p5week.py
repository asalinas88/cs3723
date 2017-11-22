#! /usr/bin/python

import re
from p5team import teamNames, collegeScore, printTeams

# declare wins/losses arrays
win = ["BEAT", "BEATS", "DEFEAT", "DEFEATS", "DEFEATED", "SLAUGHTER", "SLAUGHTERS", "SLAUGHTERED", "WHIP", "WHIPS", "WHIPPED", "TOP", "TOPS", "TOPPED", "UPSET", "UPSETS", "SHUT", "SHUTS", "SHUTS OUT", "EATS", "EATS", "DESTROY", "DESTROYS", "DESTROYED", "SOAR", "SOARS", "SOAR OVER", "SOARS OVER"]

loss = ["LOSES TO", "LOST TO", "BEAT BY", "BEATEN BY", "DEFEATED BY", "SLAUGHTERED BY", "WHIPPED BY", "TOPPED BY", "UPSET BY", "EATEN BY", "DESTROYED BY"]

def sortbyPosition(team):
    return team[1]

def findCollege(inputLine):
    collegeNames = []
    for college, teamName in teamNames.items():
        # creates an array of the college and the team names
        collegeTeams = [college] + teamName
        for team in collegeTeams:
            # finds the index position of team
            # var = the position of the index in the string (UTSA is at [0])
            pos = inputLine.find(team)
            # if the variable is there (it will NOT return -1)
            if pos is not -1:
                # add the college and its position to collegeNames dictionary
                collegeNames.append((college, pos))
                # get rid of the team name
                inputLine = inputLine.replace(team, '')
                break
    # input line is the modified input line (it removed all the team names)
    return inputLine, collegeNames
    
def processWeeks(week, weekName):
  # for each input line in the file
  for inputLine in week:
        inputLine = inputLine.rstrip('\n')
        print(">>>", inputLine)

        # if input line is ENDWEEK print the total team wins/losses
        if inputLine == "ENDWEEK":
            printTeams(weekName)
            break

        # grab the team name from the input line
        inputLine,  allTeams = findCollege(inputLine)
        allTeams.sort(key=sortbyPosition) 
        # check for team name in college dictionary
        scoreMatch = re.compile(r',?\s*(\d+).*?(\d+)')

        # grab scores as a group
        scores = scoreMatch.search(inputLine)
        scores = (scores.group(1), scores.group(2))
        # replaces inputline with an empty string
        inputLine = scoreMatch.sub('', inputLine)

        # searches if key word is in win array
        if inputLine.strip() in win:
            scores = sorted(scores, key=int, reverse=True)
            collegeScore[allTeams[0][0]][0]+=1
            collegeScore[allTeams[1][0]][1]+=1
            print("\t", allTeams[0][0], allTeams[1][0], scores[0], scores[1])
        
        # searches if key word is in loss array
        elif inputLine.strip() in loss:
            # sort the scores in reverse order
            scores = sorted(scores, key=int, reverse=True)
            # tally the wins/losses
            collegeScore[allTeams[1][0]][0]+=1
            collegeScore[allTeams[0][0]][1]+=1
            print("\t", allTeams[1][0], allTeams[0][0], scores[0], scores[1])
        else:
            if int(scores[0]) < int(scores[1]):
                scores = sorted(scores, key=int, reverse=True)
                collegeScore[allTeams[1][0]][0]+=1
                collegeScore[allTeams[0][0]][1]+=1
                print("\t", allTeams[1][0], allTeams[0][0], scores[0], scores[1])
            else:
                scores = sorted(scores, key=int, reverse=True)
                collegeScore[allTeams[0][0]][0]+=1
                collegeScore[allTeams[1][0]][1]+=1
                print("\t", allTeams[0][0], allTeams[1][0], scores[0], scores[1])
