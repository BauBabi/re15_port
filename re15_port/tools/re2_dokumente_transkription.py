#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""re2_dokumente_transkription.py — schreibt die Transkriptionen der 25 RE2-Dokumente
nach extracted_re2_dokumente/texte/transkription/FILE<NN>.txt.

⛔ WICHTIG — WAS DAS IST UND WAS NICHT:
Der Dokument-TEXT von RE2 ist auf der PSX KEIN Zeichenstrom. Er ist in die 4bpp-Bilder
in COMMON/DATA/FILES.TIM gerastert: die 16-Farben-CLUT einer Textseite hat Eintrag 0 =
0x0000 (PSX-GPU: Texel 0x0000 ist vollstaendig durchsichtig, psx-spx "Texture Color
0000h") und Eintrag 9 = 0x7FFF = weiss — es sind also weisse Glyphen auf durchsichtigem
Grund, kein Text. Als echte Zeichenketten existieren in der EXE nur die Dokument-NAMEN
(Bank @0x8009E550/@0x8009EBAC, siehe re2_dokumente_extrakt.py).

Die Texte hier sind deshalb eine **von Hand abgelesene Transkription** der gerenderten
Seitenbilder aus extracted_re2_dokumente/hintergruende/seiten_lesbar/. Sie sind
KEINE Originalbytes und koennen Lesefehler enthalten. Das Original ist immer das Bild.
Die Zeilenumbrueche folgen dem Bild (RE2 bricht die Zeilen im Bild fest um).

Seitenzaehlung wie im Extraktor: `TITEL` = das Bild im Titel-Slot first[doc]; `S00` ist
mit `TITEL` byte-identisch (in allen 25 Dokumenten geprueft) und deshalb hier nicht
doppelt aufgefuehrt; `S01`..`Snn` sind die Folgeseiten aus den Slots first[doc]+1+p.
"""
import os

HERE = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(HERE, "..", ".."))
OUT = os.path.join(REPO, "extracted_re2_dokumente", "texte", "transkription")

LEER = "(leer — Seitenbild enthaelt nur Index 0, also nichts Sichtbares)"

T = {}

T[0] = ("CHRIS's diary", [
    ("TITEL", "CHRIS'S DIARY"),
    ("S01", "August 8th\n"
            "I talked to the chief today\n"
            "once again, but he refused to\n"
            "listen to me. I know for\n"
            "certain that Umbrella\n"
            "conducted T-virus research\n"
            "in that mansion.\n"
            "Anyone infected turns into\n"
            "a zombie."),
    ("S02", "But the entire mansion went up\n"
            "in that explosion; along with\n"
            "any incriminating evidence.\n"
            "Since Umbrella employs so many\n"
            "people in town, no one is\n"
            "willing to talk about\n"
            "the incident.\n"
            "It looks like I'm running out\n"
            "of options."),
    ("S03", "August 17th\n"
            "We've been receiving a lot of\n"
            "local reports about strange\n"
            "monsters appearing at\n"
            "random throughout the city.\n"
            "This must be the work\n"
            "of Umbrella."),
    ("S04", "August 24th\n"
            "With the help of Jill and\n"
            "Barry, I finally obtained\n"
            "information vital to this case.\n"
            "Umbrella has begun research\n"
            "on the new G-virus, a variation\n"
            "of the original T-virus.\n"
            "Haven't they done enough\n"
            "damage already?!"),
    ("S05", "We talked it over, and have\n"
            "decided to fly to the main\n"
            "Umbrella HQ in Europe. I won't\n"
            "tell my sister about this trip\n"
            "because doing so could put her\n"
            "in danger.\n"
            "Please forgive me Claire."),
])

T[1] = ("Mail to Chris", [
    ("TITEL", "FEDERAL POLICE DEPT.\n-INTERNAL INVESTIGATION\nREPORT"),
    ("S01", "Mr. Chris Redfield\n"
            "Raccoon City Police Dept.\n"
            "S.T.A.R.S. division\n"
            "\n"
            "As per your request, we\n"
            "have conducted our\n"
            "internal investigation and\n"
            "discovered the following\n"
            "information:"),
    ("S02", "1) Regarding the G-virus\n"
            "currently under development\n"
            "by Umbrella Inc.\n"
            "\n"
            "So far it is unconfirmed that\n"
            "the G-virus even exists.\n"
            "We're continuing with our\n"
            "investigation."),
    ("S03", "2) Regarding Mr. Brian Irons,\n"
            "Chief of the Raccoon City\n"
            "Police Dept.\n"
            "\n"
            "Mr.Irons has allegedly\n"
            "received a large sum of funds\n"
            "in bribes from Umbrella Inc.\n"
            "over the last five years.\n"
            "He was apparently involved in\n"
            "the cover up of the mansion\n"
            "lab case along with several"),
    ("S04", "other incidents in which\n"
            "Umbrella appears to have\n"
            "direct involvement.\n"
            "Mr.Irons had been arrested\n"
            "under suspicion of rape on\n"
            "two separate counts during\n"
            "his years as a university\n"
            "student. He underwent\n"
            "psychiatric evaluation as a\n"
            "result of the charges but was\n"
            "released due to circumstantial"),
    ("S05", "evidence as well as his\n"
            "phenomenal academic standing.\n"
            "\n"
            "As such, extreme caution is\n"
            "advised when dealing with him.\n"
            "\n"
            "        Jack Hamilton,\n"
            "        Section Chief\n"
            "        Internal Investigations\n"
            "        United States Federal\n"
            "        Police Department"),
])

T[2] = ("Memo to LEON", [
    ("TITEL", "MEMO TO LEON"),
    ("S01", "To Leon S. Kennedy,\n"
            "Congratulations on your\n"
            "assignment to the Raccoon City\n"
            "police department.\n"
            "We all look forward to having\n"
            "you as a part of our team and\n"
            "promise to take good care of\n"
            "you.\n"
            "Welcome aboard!\n"
            "        From all the guys\n"
            "          at the R.P.D."),
])

T[3] = ("Police memorandum", [
    ("TITEL", "POLICE MEMORANDUM"),
    ("S01", "8/23/1998\n"
            "This letter is just to inform\n"
            "everyone about the recent\n"
            "movement of equipment that\n"
            "has happened during the\n"
            "precinct's rearrangement.\n"
            "\n"
            "\n"
            "The safe with four digit lock\n"
            "has been moved from the\n"
            "S.T.A.R.S. office on the second\n"
            "floor, to the eastern office"),
    ("S02", "on the first floor.\n"
            "\n"
            "          \"2236\"\n"
            "\n"
            "   Raccoon Police Liaison Dept."),
])

T[4] = ("Operation report 1", [
    ("TITEL", "OPERATION REPORT"),
    ("S01", "         -Operation Report-\n"
            "               September 26th\n"
            "The Raccoon Police Dept. was\n"
            "unexpectedly attacked by\n"
            "zombies. Many have been\n"
            "injured. Even more were killed.\n"
            "During the attack, our\n"
            "communications equipment was\n"
            "destroyed and we no longer\n"
            "have contact with the outside."),
    ("S02", "We have decided to carry out\n"
            "an operation with the intent\n"
            "of rescuing any possible\n"
            "survivors as well as to\n"
            "prevent this disaster from\n"
            "spreading beyond Raccoon City.\n"
            "The details of the operation\n"
            "are as follows:"),
    ("S03", "Security of armaments and\n"
            "ammunition.\n"
            "\n"
            "Chief Irons has voiced concern\n"
            "regarding the issue of\n"
            "terrorism due to a series of\n"
            "recent unresolved incidents.\n"
            "On the very day before the\n"
            "zombies' attack, he made the\n"
            "decision to relocate all\n"
            "weapons to scattered"),
    ("S04", "intervals throughout the\n"
            "building as a temporary\n"
            "measure to prevent their\n"
            "possible seizure.\n"
            "Unfortunately, this decision\n"
            "has made it extremely\n"
            "difficult for us to locate all\n"
            "ammunition caches.\n"
            "It has become our top priority\n"
            "to recover these scattered\n"
            "munitions."),
    ("S05", "To unlock the weapon storage.\n"
            "\n"
            "As stated earlier, it will be\n"
            "extremely difficult to\n"
            "secure all the ammunition.\n"
            "However, a considerable supply\n"
            "still remains in the\n"
            "underground weapon storage.\n"
            "Unfortunately, the person in\n"
            "charge of the card key used to\n"
            "access the weapon storage is"),
    ("S06", "missing and we have been\n"
            "unable to locate the key.\n"
            "One of the breakers went\n"
            "down during the battle and the\n"
            "electronic locks are not\n"
            "functioning in certain areas.\n"
            "It has become a top priority to\n"
            "restore the power in the power\n"
            "room and secure those locks.\n"
            "\n"
            "     Recorder: David Ford"),
    ("S07", "         -Operation Report-\n"
            "               September 27th\n"
            "1:00PM. The west barricade\n"
            "has been broken through and\n"
            "another exchange ensued.\n"
            "We sheltered the injured in\n"
            "the confiscation room on the\n"
            "first floor temporarily.\n"
            "Twelve more people were\n"
            "injured in the battle.\n"
            "     Recorder: David Ford"),
    ("S08", "      -Additional Report-\n"
            "Three additional people were\n"
            "killed following the sudden\n"
            "appearance of an as of yet\n"
            "unknown creature.\n"
            "This creature is identified\n"
            "by missing patches of skin\n"
            "and razor-like claws.\n"
            "However, its most\n"
            "distinguishing characteristic\n"
            "is its lance-like tongue,"),
    ("S09", "capable of piercing a human\n"
            "torso in an instant.\n"
            "Their numbers as well as\n"
            "their location remains\n"
            "unknown. We have tentatively\n"
            "named this creature the\n"
            "\"licker\" and are currently\n"
            "in the process of developing\n"
            "countermeasures to deal with\n"
            "this new threat."),
])

_MAIL_CHIEF = [
    ("TITEL", "MAIL TO THE CHIEF"),
    ("S01", "To:Mr. Brian Irons, Chief of\n"
            "the Raccoon City Police Dept.\n"
            "\n"
            "We have lost the mansion lab\n"
            "facility due to the actions\n"
            "of the renegade operative,\n"
            "Albert Wesker.\n"
            "Fortunately, his interference\n"
            "will have no lasting effects\n"
            "upon our continued virus\n"
            "research."),
    ("S02", "Our only present concern is\n"
            "the presence of the remaining\n"
            "S.T.A.R.S. members: Redfield,\n"
            "Valentine, Burton, Chambers\n"
            "and Vickers.\n"
            "If it comes to light that the\n"
            "S.T.A.R.S. have any evidence\n"
            "as to the activities of our\n"
            "research, dispose of them in\n"
            "such a manner that would\n"
            "appear to be purely accidental."),
    ("S03", "Continue to monitor their\n"
            "progress and make certain\n"
            "their knowledge does not go\n"
            "public.\n"
            "Annette will continue to be\n"
            "your contact throughout this\n"
            "affair.\n"
            "\n"
            "              William Birkin"),
    ("S04", "To:Mr. Brian Irons, Chief of\n"
            "the Raccoon City Police Dept.\n"
            "\n"
            "I have deposited the amount of\n"
            "US $10,000 to the account for\n"
            "your services this term as per\n"
            "our agreement.\n"
            "The development of the G-virus\n"
            "scheduled to replace the\n"
            "T-virus, is near completion.\n"
            "Once completed, I am certain"),
    ("S05", "that I will be appointed to\n"
            "be a member of the executive\n"
            "board for Umbrella Inc.\n"
            "It is imperative that we\n"
            "proceed with extreme caution.\n"
            "Redfield and the remaining\n"
            "S.T.A.R.S. members are still\n"
            "attempting to uncover\n"
            "information on the project.\n"
            "Continue to monitor their\n"
            "activities and block all"),
    ("S06", "attempts to investigate the\n"
            "underground research\n"
            "facilities.\n"
            "\n"
            "              William Birkin"),
    ("S07", "To:Mr. Brian Irons, Chief of\n"
            "the Raccoon City Police Dept.\n"
            "\n"
            "We have a problem. I have\n"
            "received information informing\n"
            "me that Umbrella HQ has sent\n"
            "spies to recover my research\n"
            "on the G-virus. There are an\n"
            "unknown number of agents\n"
            "involved. They must not be\n"
            "allowed to take this project"),
    ("S08", "away from me as it represents\n"
            "my entire life's work.\n"
            "Search the city thoroughly\n"
            "for any suspicious persons.\n"
            "Detain any such individuals by\n"
            "whatever means deemed\n"
            "necessary and contact me\n"
            "immediately through Annette.\n"
            "With these precautions, any\n"
            "possible threat should be\n"
            "eliminated."),
    ("S09", "I will not allow anyone to\n"
            "steal my work on the G-virus.\n"
            "Not even Umbrella...\n"
            "\n"
            "              William Birkin"),
]
# Dokument 5 und 6 sind in FILES.TIM BYTE-IDENTISCH (alle 10 Slots, md5-geprueft).
T[5] = ("Mail to the chief", _MAIL_CHIEF)
T[6] = ("Mail to the chief", _MAIL_CHIEF)

T[7] = ("Secretary's diary A", [
    ("TITEL", "SECRETARY'S DIARY A"),
    ("S01", "April 6th\n"
            "I accidentally moved one of\n"
            "the stone statues on the\n"
            "second floor when I leaned\n"
            "against it. When the chief\n"
            "found out about it, he was\n"
            "furious. I swear the guy\n"
            "nearly bit my head off,\n"
            "screaming at me never to"),
    ("S02", "touch the statue again.\n"
            "If it's so important,\n"
            "then maybe he shouldn't\n"
            "have put it out in the open\n"
            "like that..."),
    ("S03", "April 7th\n"
            "I heard that all the art\n"
            "pieces from the chief's\n"
            "collection are rare items,\n"
            "literally worth hundreds of\n"
            "thousands of dollars.\n"
            "I don't know which is the\n"
            "bigger mystery: where he\n"
            "finds those tacky things,"),
    ("S04", "or where he's getting the\n"
            "money to pay for them."),
    ("S05", "May 10th\n"
            "I wasn't surprised to see\n"
            "the chief come in today\n"
            "with yet another large\n"
            "picture frame in his hands.\n"
            "This time it was a really\n"
            "disturbing painting depicting\n"
            "a nude person being hanged.\n"
            "I was appalled by the"),
    ("S06", "expression on the chief's\n"
            "face as he leered at that\n"
            "painting.\n"
            " Why anyone would consider\n"
            "something like that to be\n"
            "a work of art is beyond my\n"
            "comprehension..."),
])

T[8] = ("Secretary's diary B", [
    ("TITEL", "SECRETARY'S DIARY B"),
    ("S01", "June 8th\n"
            "As I was straightening up\n"
            "the chief's room, he burst\n"
            "through the door with a\n"
            "furious look on his face.\n"
            "It's only been 2 months since\n"
            "I've started working here,\n"
            "but that's the second time\n"
            "I've seen him like this."),
    ("S02", "The last time was when I\n"
            "bumped into that statue, only\n"
            "this time he looked even more\n"
            "agitated than ever. I\n"
            "seriously thought for a\n"
            "moment that he was going to\n"
            "hurt me."),
    ("S03", "June 15th\n"
            "I finally discovered what the\n"
            "chief has been hiding all\n"
            "along... If he finds out that\n"
            "I know, my life will be in\n"
            "serious danger.\n"
            "It's getting late already.\n"
            "I'm just going to have to take\n"
            "this one day at a time..."),
    ("S04", LEER),
])

T[9] = ("Operation report 2", [
    ("TITEL", "OPERATION REPORT 2"),
    ("S01", "         -Operation Report-\n"
            "               September 28th\n"
            "Early Morning 2:30AM. Zombies\n"
            "overran the operation room\n"
            "and another battle broke out.\n"
            "We lost four more people,\n"
            "including David.\n"
            "We're down to four people,\n"
            "including myself. We failed to\n"
            "secure the weapons cache and\n"
            "hope for our survival"),
    ("S02", "continues to diminish.\n"
            "We won't last much longer...\n"
            "We agreed upon a plan to\n"
            "escape through the sewer.\n"
            "There's a path leading from\n"
            "the precinct underground to\n"
            "the sewage disposal plant.\n"
            "We should be able to access\n"
            "the sewers through there.\n"
            "The only drawback is that\n"
            "there is no guarantee the"),
    ("S03", "sewage disposal plant is free\n"
            "of any possible dangers.\n"
            "We know our chances in the\n"
            "sewers are slim, but anything\n"
            "is better than simply waiting\n"
            "here to die.\n"
            "In order to buy more time, we\n"
            "locked the only door leading\n"
            "to the underground, which is\n"
            "located in the eastern office.\n"
            "We left the key behind in the"),
    ("S04", "western office since it's\n"
            "unlikely that any of those\n"
            "creatures have the\n"
            "intelligence to find it and\n"
            "unlock the door.\n"
            "\n"
            "I pray that this operation\n"
            "report will be helpful to\n"
            "whoever may find it.\n"
            "\n"
            "     Recorder: Elliot Edward"),
    ("S05", "In order to buy more time, we\n"
            "locked the only door leading\n"
            "to the underground, which is\n"
            "located in the eastern office.\n"
            "We left the key behind in the\n"
            "western office since it's\n"
            "unlikely that any of those\n"
            "creatures have the\n"
            "intelligence to find it and\n"
            "unlock the door."),
    ("S06", "I pray that this operation\n"
            "report will be helpful to\n"
            "whoever may find it.\n"
            "\n"
            "     Recorder: David Ford"),
])

T[10] = ("User registration", [
    ("TITEL", "USER REGISTRATION"),
    ("S01", "Temporary User Registration\n"
            "for the Culture Experiment\n"
            "Room.\n"
            "\n"
            "User Name:\"GUEST\"\n"
            "Password:None\n"
            "\n"
            "Valid for 24 hours."),
])

T[11] = ("Film A", [
    ("TITEL", "FILM A"),
    ("S01", LEER),
    ("S02", "Code G Human Body Experiment\n"
            "              9/15 15:24"),
])

T[12] = ("Film B", [
    ("TITEL", "FILM B"),
    ("S01", LEER),
    ("S02", "Pictured in front of the\n"
            "Arukas tailor.\n"
            "Regressed into a zombie\n"
            "within two hours.\n"
            "\n"
            "Subject repeatedly\n"
            "complained about severe"),
    ("S03", "agitation of the\n"
            "epidermis in addition to\n"
            "feelings of nausea.\n"
            "This happened up to the moment\n"
            "he lost consciousness.\n"
            "\n"
            "     Picture by R.Lambert"),
])

T[13] = ("Film C", [
    ("TITEL", "FILM C"),
    ("S01", LEER),
    ("S02", "Development Code:T-103\n"
            " Due to accelerated metabolism\n"
            "relative to the earlier 00\n"
            "series, this subject\n"
            "possesses exemplary\n"
            "regenerative capabilities.\n"
            "           PH-X016 File Data"),
])

T[14] = ("Patrol report", [
    ("TITEL", "PATROL REPORT"),
    ("S01", "        -Patrol Report-\n"
            "  September 20th 9:30 PM\n"
            "  Reporter:Sgt.Neil Carlsen\n"
            "\n"
            "We received a report of a\n"
            "suspicious individual\n"
            "skulking around the sewers\n"
            "in the outskirts of Raccoon\n"
            "City. I searched the area"),
    ("S02", "and located the individual,\n"
            "but he ran away before I\n"
            "was able to question him."),
    ("S03", "I recovered the following items:\n"
            "\n"
            " *A small amount of C4\n"
            "  plastic explosive.\n"
            " *An electronic detonator.\n"
            " *9x19 parabellum rounds.\n"
            " *Infrared scope [broken].\n"
            "\n"
            "          End of report."),
])

T[15] = ("Watchman's diary", [
    ("TITEL", "WATCHMAN'S DIARY"),
    ("S01", "August 11th\n"
            "I finally had the chance to\n"
            "see blue skies for the first\n"
            "time in ages, but it did\n"
            "little to lift my spirits.\n"
            "I was reprimanded by the\n"
            "chief for neglecting my\n"
            "duties while I was up on the\n"
            "clock tower.\n"
            "There's only one thing I\n"
            "still don't understand:"),
    ("S02", "the chief seemed to be more\n"
            "concerned about the fact\n"
            "that I was up on the tower\n"
            "rather than that I was\n"
            "neglecting my duties.\n"
            "Why was access to the tower\n"
            "prohibited in the first\n"
            "place anyway?"),
    ("S03", "September 5th\n"
            "I recently talked to the old\n"
            "man who works in the scrap\n"
            "yard out back. His name is\n"
            "Thomas. He's a quiet man and\n"
            "really seems to enjoy chess.\n"
            "He even went so far as to\n"
            "design a special key and lock\n"
            "engraved with chess pieces on\n"
            "them for one of the doors in\n"
            "the disposal yard."),
    ("S04", "We made plans to play chess\n"
            "tomorrow night. I can't help\n"
            "but wonder how good he is.\n"
            "One thing that's been\n"
            "bothering me about him is the\n"
            "way that he's always\n"
            "scratching himself... Does\n"
            "he have some sort of\n"
            "skin disease or he is just\n"
            "rude?"),
    ("S05", "September 9th\n"
            "Thomas was a much better\n"
            "player than I had imagined.\n"
            "I used to think of myself as\n"
            "a fairly decent player, but\n"
            "he did a pretty good job of\n"
            "humbling me.\n"
            "About the only thing I imagine\n"
            "that could match his skills in\n"
            "chess is his appetite. All the\n"
            "guy did was talk about"),
    ("S06", "food throughout the entire\n"
            "game. He sounded fairly\n"
            "healthy, but he didn't look\n"
            "quite right...\n"
            "I wonder if he's okay."),
    ("S07", "September 12th\n"
            "I was supposed to play\n"
            "another game of chess with\n"
            "Thomas, but we had to cancel\n"
            "it because he hasn't been\n"
            "feeling too well.\n"
            "He stopped by to see me, but\n"
            "I told him to go back and rest\n"
            "since he literally looked like\n"
            "the walking dead."),
    ("S08", "He insisted that he was just\n"
            "fine, but I could tell he was\n"
            "really having problems.\n"
            "Come to think of it, I haven't\n"
            "been feeling too good myself\n"
            "lately..."),
])

T[16] = ("Chief's diary", [
    ("TITEL", "CHIEF'S DIARY"),
    ("S01", "September 23rd\n"
            "It's all over. Those imbeciles\n"
            "from Umbrella have finally\n"
            "done it... Despite all their\n"
            "promises, they've ruined my\n"
            "town. Soon the streets will\n"
            "be infested with zombies. I'm\n"
            "beginning to think that I may\n"
            "even be infected myself.\n"
            "I'll kill everyone in town if\n"
            "this turns out to be true!!!"),
    ("S02", "September 24th\n"
            "I was successful in spreading\n"
            "confusion among the police as\n"
            "planned. I've made sure that\n"
            "no one from the outside\n"
            "will come to help.\n"
            "With the delays in police\n"
            "actions, no one will have the\n"
            "chance to escape my city alive.\n"
            "I've seen to it personally\n"
            "that all escape routes from"),
    ("S03", "inside the precinct have been\n"
            "cut off as well.\n"
            "There are several survivors\n"
            "still attempting to escape\n"
            "through the lower levels, but\n"
            "I'll make sure no one gets\n"
            "out."),
    ("S04", "September 26th\n"
            "I've had a change of heart\n"
            "about the remaining survivors\n"
            "inside the precinct. I've\n"
            "decided to hunt them down\n"
            "myself.\n"
            "I shot Ed in the back through\n"
            "the heart less than an hour\n"
            "ago. I watched him writhe\n"
            "in pain upon the floor in a\n"
            "pool of his own blood. The"),
    ("S05", "expression on his face was\n"
            "positively exquisite. He died\n"
            "with his eyes wide open,\n"
            "staring up at me. It was\n"
            "beautiful.\n"
            "I wonder if the mayor's\n"
            "daughter is still alive?\n"
            "I let her escape so I could\n"
            "enjoy hunting her down\n"
            "later...\n"
            "I'm going to enjoy my new"),
    ("S06", "trophy. Yes, frozen forever\n"
            "in the pose I choose\n"
            "to give her."),
])

T[17] = ("Sewer manager diary", [
    ("TITEL", "SEWER MANAGER DIARY"),
    ("S01", "June 28th\n"
            "It's been a while, but I saw\n"
            "Don today and we talked\n"
            "after completing our work.\n"
            "He told me he had been sick\n"
            "in bed until yesterday.\n"
            "It really doesn't come as much\n"
            "of a surprise given how long\n"
            "he's been working here.\n"
            "He was sweating like a horse\n"
            "and kept scratching his body"),
    ("S02", "while we were talking. I\n"
            "asked if he was hot, but he\n"
            "just looked at me funny.\n"
            "What's wrong with him anyway?"),
    ("S03", "July 7th\n"
            "Chief Irons has been\n"
            "visiting the lab quite often\n"
            "lately. I don't know what\n"
            "he's doing over there but he\n"
            "always looks grim.\n"
            "The expression on his face\n"
            "has been even more unsettling\n"
            "than usual...\n"
            "My guess is that it's because\n"
            "of Dr.Birkin's impossible"),
    ("S04", "requests. The chief has my\n"
            "sympathies though. After all\n"
            "he's done for the town, he\n"
            "doesn't deserve this."),
    ("S05", "July 21st\n"
            "I rarely drink because I'm\n"
            "on the graveyard shift, but I\n"
            "don't suppose I have much\n"
            "to complain about since\n"
            "this is how I make my living."),
    ("S06", "August 16th\n"
            "Chief Irons came in late\n"
            "today, looking grimmer than\n"
            "his usual self. I tried to\n"
            "joke with him to cheer him\n"
            "up but he wasn't amused. He\n"
            "pulled his gun and threatened\n"
            "to shoot me! I was able to\n"
            "calm him down, but that guy\n"
            "must have some serious\n"
            "problems. He knows he can't"),
    ("S07", "enter the lab without my\n"
            "help and my medal.\n"
            "This is what it means for\n"
            "the chief \"to serve\n"
            "and protect\"!?"),
    ("S08", "August 21st\n"
            "William informed me that the\n"
            "police and media have begun\n"
            "their investigation on\n"
            "Umbrella's affairs. He said\n"
            "that the investigation will\n"
            "be citywide and that there is\n"
            "a possibility they'll even\n"
            "search through the sewers.\n"
            "He asked me to suspend all\n"
            "Umbrella sewer facility"),
    ("S09", "operations until the\n"
            "investigation has concluded.\n"
            "The sewer will still be used\n"
            "for passage, but he stressed\n"
            "that I have to be extremely\n"
            "cautious and that I'd lose\n"
            "my job if anyone finds out\n"
            "about this operation."),
])

T[18] = ("Sewer manager fax", [
    ("TITEL", "SEWER MANAGER FAX"),
    ("S01", "      -User List of the\n"
            "       Connecting Facility-\n"
            "\n"
            "On the first and third\n"
            "Wednesdays of the month,\n"
            "Angelica Margaret, chief of\n"
            "maintenance, will make use\n"
            "of the facilities. Be sure\n"
            "to reduce the moisture levels\n"
            "in the facility by activating\n"
            "the fan, as the equipment she"),
    ("S02", "will be using is susceptible\n"
            "to the effects of water\n"
            "vapors.\n"
            "\n"
            "On the 28th of every month,\n"
            "the chemical transporter\n"
            "Don Weller will use the\n"
            "facility. The chemicals he\n"
            "will be transporting are\n"
            "extremely volatile.\n"
            "Extreme caution should be"),
    ("S03", "observed throughout their\n"
            "transport.\n"
            "\n"
            "On the 6th and 16th of every\n"
            "month, police chief Brian Irons\n"
            "will visit the facility to\n"
            "attend the regular meetings\n"
            "that take place in the lab.\n"
            "\n"
            "On the fourth Friday of every\n"
            "other month, William Birkin will"),
    ("S04", "use the facility to conduct a\n"
            "training seminar for the\n"
            "Chicago branch of Umbrella Inc.\n"
            "As the probability of an\n"
            "attack upon Dr. Birkin will\n"
            "be high, take every measure\n"
            "conceivable to guard his life.\n"
            "\n"
            "You will be informed of all\n"
            "other potential visitors and\n"
            "the times they will arrive as"),
    ("S05", "needed. Guide these individuals\n"
            "to their destination safely.\n"
            "We expect nothing but the\n"
            "best from you.\n"
            "\n"
            "       Charles Coleman\n"
            "       Secretary Chief\n"
            "       Umbrella Headquarters"),
])

T[19] = ("Film D", [
    ("TITEL", "RECRUIT"),
    ("S01", LEER),
])

T[20] = ("Vaccine synthesis", [
    ("TITEL", "INSTRUCTIONS FOR SYNTHESIS\n"
              "   OF THE G-VIRUS ANTIGEN:\n"
              "G-VACCINE. CODE NAMED \"DEVIL.\""),
    ("S01", "Any beings infected by the\n"
            "G-virus will reproduce\n"
            "through the impregnation of\n"
            "an embryo within another\n"
            "living being.\n"
            "Unless rejected by the host,\n"
            "the embryo will undertake\n"
            "a process of gradual cellular\n"
            "invasion, infecting the host's\n"
            "cells on a molecular level as\n"
            "it rewrites their DNA."),
    ("S02", "Once the metamorphosis is\n"
            "complete, the host will be\n"
            "capable of continuing this\n"
            "cycle of self-replication.\n"
            "The duration of time for the\n"
            "process to run its course will\n"
            "vary from subject to subject.\n"
            "In the early stages of\n"
            "cellular invasion, it is\n"
            "possible to halt progression\n"
            "of the metamorphosis through"),
    ("S03", "the administration of the\n"
            "G-virus antigen.\n"
            "The following procedure\n"
            "details its synthesis."),
    ("S04", "The vaccine creation requires\n"
            "the base vaccine. This can be\n"
            "arranged by the activator\n"
            "VAM. First set the empty\n"
            "cartridge to the VAM and\n"
            "activate it. After several\n"
            "moments the process will be\n"
            "complete and the white-colored\n"
            "base vaccine will be set in the\n"
            "cartridge automatically.\n"
            "Then confirm the green light"),
    ("S05", "is on, remove the cartridge,\n"
            "and proceed to the next step.\n"
            "Once the base vaccine has been\n"
            "prepared, set it in the vaccine\n"
            "synthesis machine located in\n"
            "the P-4 level experiment room.\n"
            "The machine is fully automated\n"
            "and only requires the user to\n"
            "push the sequence start\n"
            "switch. At this point,\n"
            "the program will run"),
    ("S06", "automatically and synthesis\n"
            "will be complete within\n"
            "approximately 10seconds.\n"
            "\n"
            "As the synthesis of DEVIL is\n"
            "an extremely delicate process,\n"
            "the quality will vary with\n"
            "slight shocks or changes in\n"
            "temperature. Careful handling\n"
            "is required for the proper\n"
            "results."),
])

T[21] = ("Lab security manual", [
    ("TITEL", "LABORATORY SECURITY MANUAL"),
    ("S01", "Laboratory Security Manual\n"
            "-Security measures in case of\n"
            "an emergency-\n"
            "\n"
            "In the instance of an\n"
            "uncontainable biohazardous\n"
            "breakout, all security\n"
            "measures will be directed\n"
            "toward the underground\n"
            "transport facility."),
    ("S02", "In the instance that  any\n"
            "abnormalities are detected\n"
            "among cargo in transit,\n"
            "all materials will\n"
            "automatically be transported\n"
            "from the loading zone to\n"
            "the designated high-speed\n"
            "train. At which point, all\n"
            "materials will be isolated\n"
            "and disposed of immediately."),
    ("S03", "In the instance of a Class I\n"
            "emergency, the entire train\n"
            "will be purged and disposed\n"
            "of without delay.\n"
            "\n"
            "In the instance that the lab\n"
            "itself becomes contaminated,\n"
            "the northern most route\n"
            "currently used to transport\n"
            "materials to and from the\n"
            "facility will be designated"),
    ("S04", "as the emergency escape route.\n"
            "This route will secure passage\n"
            "to the relay point outside\n"
            "the city limits.\n"
            "\n"
            "Disclosure about any\n"
            "information regarding\n"
            "research conducted\n"
            "here, or the existence of\n"
            "this facility, is strictly\n"
            "prohibited."),
    ("S05", "Since it is top priority\n"
            "to keep all research\n"
            "classified, escape access\n"
            "may be denied under certain\n"
            "extenuating circumstances."),
])

T[22] = ("P_epsilon report", [
    ("TITEL", "INVESTIGATIVE REPORT ON\n     P-EPSILON GAS"),
    ("S01", " -This report demands\n"
            "  immediate attention-\n"
            "\n"
            "The P-epsilon gas has been\n"
            "proven capable of\n"
            "incapacitating all known\n"
            "B.O.W.s (Bio Organic Weapon).\n"
            "As such, it has been designated\n"
            "for emergency usage in the\n"
            "event of a B.O.W. escape.\n"
            "Reports based upon data"),
    ("S02", "collected during prior incidents\n"
            "indicate the potential for\n"
            "negative side effects.\n"
            "\n"
            "The P-epsilon gas has been\n"
            "proven to weaken B.O.W.s'\n"
            "cellular functions. However,\n"
            "prolonged or repeated\n"
            "exposures will result in the\n"
            "creation of adaptive antibodies\n"
            "to the agent."),
    ("S03", "Furthermore, some species\n"
            "have been observed to absorb\n"
            "the P-epsilon gas as a source\n"
            "of nutrition and use the\n"
            "toxins extracted against\n"
            "anything perceived as a threat.\n"
            "\n"
            "Use of the P-epsilon gas should\n"
            "be severely limited to extreme\n"
            "cases only."),
    ("S04", "We strongly request the\n"
            "authority to re-evaluate the\n"
            "P-epsilon gas deployment\n"
            "system. We would like this\n"
            "re-evaluation to take place\n"
            "immediately.\n"
            "\n"
            "   2nd R&D Room/Security Team"),
])

T[23] = ("Rookie files (A)", [
    ("TITEL", "HINT FILES FOR THE ROOKIE MODE"),
    ("S01", "A statue of a woman bearing a\n"
            "water vessel.\n"
            "\n"
            "There's an inscription and a\n"
            "hole on the pedestal..."),
    ("S02", "The statue holding a red jewel...\n"
            "\n"
            "The two statues against each\n"
            "wall appear to have been moved\n"
            "before. Police staff have been\n"
            "told not to move either unless\n"
            "necessary."),
    ("S03", "The water tank on the roof.\n"
            "\n"
            "The tank ruptured on one\n"
            "occasion when the regulator\n"
            "valve was tightened too much.\n"
            "The valve handle was removed\n"
            "after the accident."),
    ("S04", "A gas light in the press room.\n"
            "\n"
            "The main fire must be lit before\n"
            "turning on the gas lights.\n"
            "Turn them on in the correct\n"
            "order or they'll go out."),
    ("S05", "Clock tower\n"
            "\n"
            "The ladder must be lowered to\n"
            "gain access to the upper level.\n"
            "The clock has not been\n"
            "functioning since one of the\n"
            "gears was lost."),
    ("S06", "Sewage disposal plant door.\n"
            "\n"
            "The lock device is supposedly\n"
            "shaped like a chess board\n"
            "because the manager loves\n"
            "chess obsessively."),
    ("S07", "A waterfall blocking the sewer.\n"
            "\n"
            "The water flow must be blocked\n"
            "to allow access to the other\n"
            "side of the waterfall.\n"
            "The small device on the wall\n"
            "off to the side is probably\n"
            "the key..."),
    ("S08", "The ventilation fan and mobile\n"
            "bridge in the sewer.\n"
            "\n"
            "Both devices are operated with\n"
            "the use of special handles\n"
            "that are usually detached."),
    ("S09", "The train turn table at the\n"
            "factory site.\n"
            "\n"
            "The control panel for the turn\n"
            "table is set on the turn\n"
            "table itself.\n"
            "A custom key is required to\n"
            "activate it. Access is\n"
            "strictly prohibited."),
    ("S10", "Restoring the power to the lab.\n"
            "\n"
            "The shutter can't be opened\n"
            "unless the breaker system\n"
            "has been restored.\n"
            "The fuse consists of super\n"
            "conductive materials that can\n"
            "withstand high electrical\n"
            "output and must operate at\n"
            "extremely low temperatures."),
    ("S11", "An insulated case is required\n"
            "during its transport.\n"
            "Low temperature conditions\n"
            "must be maintained during\n"
            "their installation."),
    ("S12", "How to use the computer in\n"
            "the lab.\n"
            "\n"
            "When typing the letter \"A\",\n"
            "move the cursor to \"A\" and\n"
            "then push the action button.\n"
            "When typing multiple letters,\n"
            "move the cursor over \"*\" and\n"
            "then push the action button."),
    ("S13", "Elevator leading to the\n"
            "transport facility on the\n"
            "bottom floor.\n"
            "\n"
            "The transport facility and the\n"
            "lab are connected by a large\n"
            "elevator lift located at the\n"
            "warehouse.\n"
            "It can be reached from the\n"
            "passageway to the south of the\n"
            "control room [downward on the"),
    ("S14", "map]. A key is needed to\n"
            "open the shutter."),
    ("S15", "Transport train on the bottom\n"
            "floor of the lab.\n"
            "\n"
            "The emergency power has not\n"
            "been activated. The high\n"
            "output generator will require\n"
            "the appropriate plug before\n"
            "it can be operated.\n"
            "The tunnel gate can't be\n"
            "raised until power has been\n"
            "supplied."),
])

T[24] = ("Rookie files (B)", [
    ("TITEL", "HINT FILES FOR THE ROOKIE MODE"),
    ("S01", "A statue of a woman bearing a\n"
            "water vessel.\n"
            "\n"
            "There's an inscription and a\n"
            "hole on the pedestal...\n"
            "\n"
            "The statue holding a red jewel...\n"
            "\n"
            "The two statues against each\n"
            "wall appear to have been moved\n"
            "before. Police staff have been"),
    ("S02", "told not to move either unless\n"
            "necessary.\n"
            "\n"
            "The water tank on the roof.\n"
            "\n"
            "The tank ruptured on one\n"
            "occasion when the regulator\n"
            "valve was tightened too much.\n"
            "The valve handle was removed\n"
            "after the accident."),
    ("S03", "The door that has been\n"
            "destroyed by the helicopter.\n"
            "\n"
            "The only way to open the\n"
            "sealed door is with explosives.\n"
            "There is a report that\n"
            "recently confiscated\n"
            "explosives have been stored\n"
            "on the premises."),
    ("S04", "A picture in the chief's room.\n"
            "\n"
            "Word around the campfire is\n"
            "that there is a reason why the\n"
            "chief collects all these morose\n"
            "pictures other than as a hobby.\n"
            "He supposedly becomes furious\n"
            "if anyone touches them. He's\n"
            "even erupted at the cleaning\n"
            "staff."),
    ("S05", "A gas light in the press room.\n"
            "\n"
            "The main fire must be lit before\n"
            "turning on the gas lights.\n"
            "Turn them on in the correct\n"
            "order or they'll go out.\n"
            "\n"
            "Clock tower\n"
            "\n"
            "The ladder must be lowered to\n"
            "gain access to the upper level."),
    ("S06", "The clock has not been\n"
            "functioning since one of the\n"
            "gears was lost.\n"
            "\n"
            "A waterfall blocking the sewer.\n"
            "\n"
            "The water flow must be blocked\n"
            "to allow access to the other\n"
            "side of the waterfall.\n"
            "The small device on the wall off\n"
            "to the side is probably the key..."),
    ("S07", "The ventilation fan and mobile\n"
            "bridge in the sewer.\n"
            "\n"
            "Both devices are operated with\n"
            "the use of special handles\n"
            "that are usually detached.\n"
            "\n"
            "The train turn table at the\n"
            "factory site.\n"
            "\n"
            "The control panel for the turn"),
    ("S08", "table is set on the turn\n"
            "table itself.\n"
            "A custom key is required to\n"
            "activate it. Access is\n"
            "strictly prohibited.\n"
            "\n"
            "Restoring the power to the lab.\n"
            "\n"
            "The shutter can't be opened\n"
            "unless the breaker system\n"
            "has been restored."),
    ("S09", "The fuse consists of super\n"
            "conductive materials that can\n"
            "withstand high electrical\n"
            "output and must operate at\n"
            "extremely low temperatures.\n"
            "An insulated case is required\n"
            "during its transport.\n"
            "Low temperature conditions\n"
            "must be maintained during\n"
            "their installation."),
    ("S10", "How to use the computer in\n"
            "the lab.\n"
            "\n"
            "When typing the letter \"A\",\n"
            "move the cursor to \"A\" and\n"
            "then push the action button.\n"
            "When typing multiple letters,\n"
            "move the cursor over \"*\" and\n"
            "then push the action button."),
    ("S11", "Vaccine synthesis.\n"
            "\n"
            "Vaccine synthesis is a\n"
            "delicate process.\n"
            "Find the \"cartridge\"\n"
            "containing the vaccine\n"
            "precursor components and\n"
            "create the \"base vaccine\"\n"
            "using the activator -VAM-\n"
            "\n"
            "Place this into the synthesis"),
    ("S12", "machine in the lab to prepare\n"
            "the completed vaccine.\n"
            "\n"
            "Elevator leading to the\n"
            "transport facility on the\n"
            "bottom floor.\n"
            "\n"
            "The transport facility and the\n"
            "lab are connected by a large\n"
            "elevator lift located at the\n"
            "warehouse."),
    ("S13", "It can be reached from the\n"
            "passageway to the south of the\n"
            "control room [downward on the\n"
            "map]. A key is needed to\n"
            "open the shutter.\n"
            "\n"
            "Transport train on the bottom\n"
            "floor of the lab.\n"
            "\n"
            "The emergency power has not\n"
            "been activated. The high"),
    ("S14", "output generator will require\n"
            "the appropriate plug before\n"
            "it can be operated.\n"
            "The tunnel gate can't be\n"
            "raised until power has been\n"
            "supplied."),
    ("S15", "machine in the lab to prepare\n"
            "the completed vaccine."),
])

KOPF = """RE2 (Retail, Leon) — Dokument %(n)d: %(titel)s
Item-Id 0x%(id)02X   Dokument-Index %(n)d

⛔ TRANSKRIPTION — von Hand aus den Seitenbildern abgelesen, KEINE Originalbytes.
   Das Original ist das Bild: extracted_re2_dokumente/hintergruende/seiten_lesbar/
   FILE%(n)02d_*.png   (Rohdaten: hintergruende/seiten_roh/FILE%(n)02d_*.TIM)
   In RE2 ist der Dokumenttext auf der PSX kein Zeichenstrom, sondern in 4bpp-Bilder
   gerastert. Nur der Dokument-NAME existiert als Zeichenkette (texte/FILE%(n)02d_name.txt).
   `TITEL` ist byte-identisch mit Seite S00 und daher nur einmal aufgefuehrt.
"""


def main():
    os.makedirs(OUT, exist_ok=True)
    assert sorted(T) == list(range(25)), "es fehlen Dokumente: %s" % (
        sorted(set(range(25)) - set(T)),)
    total = 0
    for n in range(25):
        titel, seiten = T[n]
        lines = [KOPF % dict(n=n, titel=titel, id=0x68 + n)]
        for tag, txt in seiten:
            lines.append("\n=== %s ===\n%s\n" % (tag, txt))
            total += 1
        p = os.path.join(OUT, "FILE%02d.txt" % n)
        open(p, "w", encoding="utf-8", newline="\n").write("".join(lines))
    print("25 Transkriptionen, %d Seiten -> %s" % (total, OUT))


if __name__ == "__main__":
    main()
