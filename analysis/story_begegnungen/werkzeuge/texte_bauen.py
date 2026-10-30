# Baut analysis/story_begegnungen/TEXTE_40.md aus den dekodierten Disc-Texten (texte40.json), dem Ausloese-Zensus
# (used.json) und der Uebersetzung unten. Japanisch kommt ungefiltert aus der Disc, nie von Hand.
import json, re, sys
sp, ziel = sys.argv[1], sys.argv[2]
rows = json.load(open(sp + r'\texte40.json', encoding='utf-8'))
used = json.load(open(sp + r'\used.json'))
T = {}
for r, i, t in rows:
    if i is None:
        continue
    t = re.sub(r'⟦END..⟧', '', t); t = re.sub(r'⟦WAIT..⟧', ' ', t); t = t.replace('⟦▼⟧', ' ▼ ')
    t = re.sub(r'(⟦YN⟧).*$', ' [Ja/Nein]', t); t = re.sub(r' +', ' ', t).strip()
    T[(r, i)] = t

ORT = {
 '102': 'Büro mit Schreibtischen und Blutspuren', '104': 'Gang', '109': 'Hofbereich mit großem Behälter, verschlossene Tür',
 '10B': 'Zufahrt mit Transporter „RACCOON POLICE“', '115': 'Büro des Polizeichefs mit Stadtmodell', '117': 'Dach, Hubschrauber-Landeplatz',
 '119': 'Flur am Schießstand (Bahnen 3/4)', '11B': 'Garage mit Gittertor, Rolltor, grüner Boden, Kanaldeckel', '11C': 'Parkgarage mit Streifenwagen',
 '207': 'Kanaltunnel', '300': 'Hof mit Umbrella-Lkw vor der Fabrik', '301': 'Straße, Blutlachen', '306': 'Hinterhof am Zaun',
 '307': 'Halle mit Lastenaufzug', '308': 'Stahlträger-Halle, grüne Tür', '309': 'Grün gestrichener Raum mit Aushängen',
 '30E': 'Halle hinter Gitterzaun', '400': 'Eingangshalle des Labors', '401': 'Laborbüro mit Liege und Monitoren', '403': 'Achteckige Schalthalle',
 '40A': 'Medizinraum mit CT', '501': 'Computerraum', '504': 'Blauer Laborgang', '506': 'P-4-Labor, rotes Licht', '50A': 'Laborgang mit Lichtrahmen',
 '512': 'Brennender Laborgang', '514': 'Laborgang',
}
D = {
'102': {0:('John','Nein, das kann nicht… Roy, das ist doch nicht wahr?'),1:('Roy','John… hilf mir… töte mich.'),
 2:('Elza','John, schieß schnell! Das ist nicht mehr Roy!!'),3:('John','Roy, ich bin es… du erkennst mich doch?'),
 4:('Elza','John, über Funk konnte ich keine Hilfe rufen. Der Strom im Gebäude ist ausgefallen…'),5:('Elza','Allein weiß ich nicht, was ich tun soll.'),
 6:('Elza','Wir, die noch übrig sind, müssen etwas unternehmen. Sonst werden wir auch noch…'),7:('John','Bitte… lass mich… allein…')},
'104': {0:('System','Da ist ein Knopf. Drücken?')},
'109': {0:('Ada','Jemand! Jemand soll hier aufmachen!'),1:('Leon','He! Ist da drin jemand?'),2:('Ada','Jemand! Ist denn niemand da! Falls doch, macht auf!'),
 3:('Leon','So! Alles in Ordnung?'),4:('Ada','Gerettet… danke, ich stehe in deiner Schuld. [Kanji-Lücke]'),
 5:('Leon','Reden wir später… hier ist es gefährlich. Und die Wunde muss versorgt werden. Gehen wir.')},
'10B': {0:('Elza','John, warte! Im Revier ist noch ein Mädchen zurückgeblieben.'),1:('John','Was?'),
 2:('Elza','Sie war in der Tiefgarage eingesperrt. Ich kann sie nicht zurücklassen. [Kanji-Lücken: 駐車場, 閉, 込]'),
 3:('John','Die Tiefgarage… dann ist es hier entlang kürzer. Komm schnell. [Lücken ergänzt]')},
'115': {0:('Polizeichef','Mir geht es gut. Denk jetzt an deine eigene Sicherheit.')},
'117': {0:('John','Elza, beeil dich!'),1:('John','Wir müssen das Ding hier runterstoßen! Hilf mir!'),2:('John','Los, hilf mir!'),3:('John','Los, hier runter!')},
'119': {0:('System','Da ist ein Schalter. Drücken?'),1:('System','Da liegt ein Schraubendreher. Nehmen?')},
'11B': {0:('Elza','Du! Was machst du denn an so einem Ort?'),1:('Elza','Du bist hier eingesperrt, nicht wahr? Komm her…'),2:('Elza','Wie heißt du?'),
 3:('Sherry','…Sherry.'),4:('Elza','Sherry heißt du? Ein süßer Name. …Ich bin Elza.'),5:('Sherry','Elza…'),6:('Elza','Genau! …Hör zu, Sherry.'),
 7:('Elza','Warte hier. Ich hole dich später ganz sicher raus.'),8:('Elza','Warte brav im Wagen, ja?'),9:('Leon','H-he! (ruft der vorbeilaufenden Sherry nach)'),
 10:('Marvin','Leon! Du lebst! Was war das für ein Lärm oben?'),11:('Leon','Das Rolltor wurde durchbrochen. Sie kommen einer nach dem anderen rein.'),
 12:('Marvin','Und der Chef? [Kanji-Lücke 署]'),13:('Marvin','Verstehe… verdammt!'),14:('Marvin','Hast du gesehen? Mit dem Streifenwagen kommen wir nicht mehr raus. Was nun?'),
 15:('Leon','Marvin, komm her!'),16:('Leon','Hier drunter ist ein Kanaldeckel. Wie wäre es, wenn wir dort in den Untergrund fliehen?'),
 17:('Marvin','Einen Versuch ist es wert. Gut, schieben wir das Ding weg. [Kanji-Lücke 価値]'),18:('Marvin','Leon, hilf mit!'),19:('Marvin','Los, schnell!'),
 20:('Leon','Marvin, ich gehe zuerst runter. Danach lass die Frau runter!'),21:('Marvin','OK. Pass auf dich auf!'),22:('System','Allein lässt sich das nicht schieben.')},
'11C': {0:('Leon','Ada! Wo ist Marvin? War er nicht bei dir?'),1:('Ada','Er ist unterwegs umgekehrt und wollte dich suchen.'),2:('Ada','Vorhin, als es oben so laut krachte.'),
 3:('Ada','Bist du ihm nicht begegnet?'),4:('Leon','Was!'),5:('Leon','Ada, versteck dich im Streifenwagen.'),6:('Ada','J-ja, das tue ich…'),7:('Leon','So, das war es.'),
 8:('Ada','…was für ein Ungeheuer hier herumläuft.'),9:('Leon','Ja. Hauptsache, wir kommen hier raus.'),10:('Leon','Ja. Hauptsache, wir kommen hier raus. (Dublette)')},
'207': {0:('System','Ein Schalter für das Rolltor. Drücken?')},
'300': {0:('System','Unter dem Kanaldeckel führt eine Leiter hinab. Hinabsteigen?'),1:('Spielfigur PL0D (Marvin)','Meine Kräfte reichen nicht mehr. Schnell zu den Gefährten.'),
 2:('Leon','Marvin! Das sind doch… Polizisten aus Raccoon!'),3:('Marvin','Ja, furchtbar… alles Ermittler im Umbrella-Fall.'),
 4:('Marvin','In dieser stillgelegten Fabrik ist etwas… Leon, sei vorsichtig!'),5:('Leon','Marvin, wo ist die Frau?'),6:('Marvin','Ada? Die ist schon vorgegangen.'),
 7:('Marvin','Bei diesem Anblick hat sie nicht mal die Miene verzogen.'),8:('Leon','Beim nächsten Treffen werde ich sie einiges fragen müssen…'),9:('—','!!'),
 10:('Elza','W-was ist das hier…?'),11:('John','Wie du siehst: tote Polizisten.'),12:('John','Das hier soll mal eine Chemiefabrik gewesen sein, aber die dürfte längst stillliegen…'),
 13:('John','Was ist hier bloß passiert?'),14:('Sherry','Elza…'),15:('Elza','Keine Angst, bleib dicht bei mir.'),16:('John','Gehen wir. Erst mal ein Versteck suchen.')},
'301': {0:('System','Verriegelt. Zum Öffnen braucht man eine Karte.'),1:('System','Mit der ID-Karte entriegelt.')},
'306': {0:('System','Abgeschlossen.'),1:('Elza','He, Sherry!'),2:('Elza','Was ist denn? Rennst einfach los…'),3:('Sherry','…Elza, ich kenne einen geheimen Eingang.'),
 4:('Elza','Einen geheimen Eingang?'),5:('Sherry','Hier. Da drin kommt man rein.'),6:('Elza','Sherry! Warte!'),7:('Sherry','Das Schloss an der Hintertür mache ich von innen auf. Wartet an der Tür!'),
 8:('John','He, Kleine! Das ist gefährlich, komm zurück!'),9:('Leon','Irgendwann wurde das Schloss entfernt…')},
'307': {0:('Ada','Steigt in diesen Aufzug! …Etwas anderes bleibt nicht.'),1:('Leon','Hier rein? …Wohin führt der denn?'),2:('Ada','In die Hölle… vielleicht wäre es besser, sich hier von denen fressen zu lassen…'),
 3:('Leon','Was?'),4:('Marvin','Schnell, sie kommen!'),5:('Elza','Stimmt. Es ist bestimmt irgendwo hier…!'),6:('John','OK Elza, den Rücken decke ich dir.'),7:('Elza','Es kam aus diesem Aufzug.'),
 8:('John','…Riesiges Ding. Damit wurden wohl sehr große Lasten transportiert. …Kriegst du ihn in Gang?'),9:('Elza','Nein… für Außenstehende ist das wohl unmöglich…'),
 10:('Sherry','Ich weiß, wie man ihn bedient!'),11:('Elza','Sherry? …Wer bist du eigentlich…'),12:('John','He, reden wir später. Die wollen bis hier rein…! Wenn der Aufzug läuft, fahren wir runter.'),
 13:('Elza','Was redest du da! Von hier ist ein unbekanntes Monster gekommen!?'),14:('John','Bleiben wir hier, sind wir deren Futter! Es geht nicht anders. Los.'),
 15:('System','Anscheinend kein Strom… Liefe die Steuerbox, würde der Aufzug wohl funktionieren…')},
'308': {0:('Marvin oder Ada','…hartnäckiger Kerl…'),1:('Leon oder Ada','…das ist… also doch…')},
'309': {0:('System','Das Licht ist bereits an…'),1:('System','Das Licht geht nicht an… Ist der Schutzschalter oben?'),2:('System','Der Schalter scheint unten zu sein…'),3:('System','Ich komme nicht an den Schalter…'),
 4:('System','Der Schalter scheint unten zu sein…'),5:('System','Der Schalter ist oben… Umlegen?'),6:('System','Ein Riegel liegt vor, die Tür geht nicht auf. Zum Lösen braucht man einen Schlüssel…'),
 7:('System','Ein Riegel liegt vor, die Tür geht nicht auf.'),8:('Sherry (spielbar)','Ich muss Elza und die anderen schnell retten…'),
 9:('John','Puh… das war knapp! Fast hätten sie mir in den Hintern gebissen. Alles klar, Elza?'),10:('Elza','Puh… geht so! Ich dachte schon, das war es!'),
 11:('John','He, Kleine, danke! Das vergesse ich dir nicht!'),12:('Elza','Wirklich, danke. Puh, du hast uns gerettet.'),13:('John','Wie heißt du denn, Kleine?'),14:('Sherry','Sherry…'),
 15:('John','Sherry also! Ich bin John. Ab jetzt beschütze ich dich, egal was kommt!'),16:('Elza','Du bist ja ganz vernarrt.'),
 17:('John','Na klar, sie hat mir das Leben gerettet! Sherry beschütze ich, und wenn es mein Leben kostet!'),18:('Elza','Du bist einfach gestrickt… Aber Sherry, woher kennst du so einen Eingang?'),
 19:('Sherry','Hier… hat mein Papa gearbeitet. …Deshalb…'),20:('Elza','Dein… Papa…?'),21:('John','He, gehen wir langsam. Wenn wir hierbleiben, kommen die wieder in Scharen.'),
 22:('Elza','Stimmt. Teilen wir uns auf und suchen einen sicheren Ort.'),23:('John','Gut. Übernimm dich nicht. …Sherry, du kommst mit mir!'),24:('Elza','Moment, geht das auch gut?'),25:('John','Verlass dich drauf!'),
 26:('System','Riegelschlüssel aufgehoben.'),27:('System','Tür mit dem Riegelschlüssel geöffnet.')},
'30E': {0:('Ada','Was war das für ein Lärm eben?'),1:('Leon','D-du bist…!'),2:('Ada','…? Wo ist der Mann? Ist er nicht bei dir?'),
 3:('Leon','…Ä-äh, Marvin ist in der Halle. Die Bestien draußen haben ihn am Bein erwischt, er kann nicht laufen… …Ada, …was hast du die ganze Zeit gemacht!? Was ist in diesem Gebäude passiert? Und die toten Polizisten draußen!?'),
 4:('Ada','…Halle…?'),5:('Ada','Schlecht… wenn er dort bleibt… Die da draußen sammeln sich allmählich. Hinter so viel Glas hält man dieser Menge nicht stand!'),
 6:('Leon','Marvin!! Alles in Ordnung?'),7:('Leon','Diese Wunde… haben die dich erwischt? …ziemlich tief…'),8:('Marvin','…Die Halle ist verloren! Die da draußen haben den Haupteingang durchbrochen und drängen herein!'),
 9:('Leon','Was?'),10:('Marvin','Die von oben strömen zusammen! Bis hier draußen am Stadtrand… Oben ist es vorbei…'),11:('Elza','John!'),
 12:('John','Elza, hier bist du! Schlecht… die Bestien haben uns umstellt! …Verdammt, nirgends ein Ausweg… Elza, wie sieht es bei dir aus… gab es hinten im Raum was Leckeres zu essen?'),
 13:('Elza','Das ist jetzt nicht das Problem…'),14:('John','W-was ist? …Waren da auch diese Leichen…'),15:('Elza','Nein… nicht so was… Das… das war… was war das nur…'),16:('John','Was redest du? Was ist da hinten?'),
 17:('Elza','Ich weiß es nicht… aber das… das war doch kein… Mensch…'),18:('John','Elza, ist das Ding noch drin?'),19:('Elza','Ja, wohl schon… Aber als ich in Panik schoss, verschwand es plötzlich… wohin, weiß ich nicht.'),
 20:('John','…Draußen sind jedenfalls zu viele Leichen, zurück ist zu gefährlich. …Wir müssen rein.'),21:('Elza','Aber… das Monster lauert vielleicht irgendwo!'),22:('John','Wenn es nur eines ist, schaffen wir das schon… …Los!'),
 23:('System','Draußen ist es gefährlich… Ich muss schnell den Aufzug in Gang bringen.')},
'400': {0:('System','Die Tür ist verriegelt. Auf dem Panel steht: „Wegen Notfall-Ausrufung gesperrt. Zum Entriegeln ist eine ID-Karte nötig.“'),1:('System','ID-Karte benutzen?'),2:('System','„ID bestätigt, entriegle.“'),
 3:('Leon','Ada, was ist das hier…?'),4:('Ada','Egal, beeil dich! Hier in den Raum!'),5:('Leon','Geht vor! Ich erledige das!'),6:('Ada','Leon…!'),7:('Leon','Egal, schnell!'),8:('Leon','Kümmere dich um Marvin!'),
 9:('Elza','Was? Was ist das hier…!?'),10:('John','Nicht schlecht…'),11:('Elza','Sherry, wo sind wir hier?'),12:('Elza','Du kennst sogar diesen Ort?'),13:('Sherry','Das ist Papas Forschungslabor. Irgendwo muss Papa sein.'),
 14:('Sherry','Ich suche ihn!'),15:('Elza','He, Sherry!'),16:('Elza','Nicht allein!'),17:('Elza','Sherry!'),18:('John','Elza, hinterher!')},
'401': {0:('Ada','Leon, diese Wunde… wenn wir uns nicht beeilen, wird es schlimm.'),1:('Leon','Ada… wer bist du eigentlich? Etwa…'),2:('Ada','Ja. Ich gehöre zu Umbrella.'),
 3:('Ada','Leon… jetzt ist keine Zeit zum Reden. Wenn wir uns nicht beeilen, stirbt er.'),4:('Leon','Was soll ich tun?'),5:('Ada','Deren Zustand kommt vom Virus. Mit einer Impfung bricht es nicht aus.'),
 6:('Leon','Wo ist der Impfstoff?'),7:('Ada','Der Impfstoff selbst ist nicht hier, aber man kann ihn aus dem „Virusstamm“ herstellen. Aber…'),8:('Leon','Was ist, Ada?'),9:('Ada','Leon, ich kann nicht helfen.'),10:('Leon','Was!?'),
 11:('Ada','Ich habe etwas zu erledigen. Ihr habt mir viel geholfen, aber das muss ich selbst tun!'),12:('Leon','Ich… mache es notfalls allein!'),13:('Leon','Sag mir, wie!'),
 14:('Ada','Im Ultratiefkühl-Labor hier muss der Virusstamm lagern. Setz ihn ins Gerät, starte das Programm, der Rest läuft automatisch. Aber der Stamm ist äußerst empfindlich. Geh falsch damit um, und alles ist hin.'),
 15:('Leon','Es bleibt nichts anderes. Marvin… ich rette dich.'),16:('Ada','Leon, wenn alles vorbei ist, geh zum unterirdischen Schutzraum! Sonst kann ich für dein Leben nicht garantieren. Das hier ist ein Umbrella-Labor, sei bloß vorsichtig.'),
 17:('Leon','He, Ada! Nicht für mein Leben garantieren…?'),18:('Leon','Ich muss den Impfstoff schnell holen!'),19:('Leon','Was ist? Marvin, wach auf!!'),20:('Leon','Ada!'),
 21:('Ada','Leon! Was machst du! Das Virus erreicht auch diesen Ort!'),22:('Ada','Schnell zum Schutzraum!'),23:('System','Eine Reihe Monitore. Kein Strom offenbar, völlig tot.'),24:('System','Auf dem Tisch liegt nichts.'),
 25:('System','Dieses Gerät wurde wohl nicht benutzt.'),26:('System','Eine Notiz: „An Michael: Ich lege die ID-Karte auf den linken Tisch. Verlier sie diesmal nicht. Henry“'),27:('System','In diesem Spind ist nichts Besonderes.'),
 28:('System','Auch dieser Gefrierschrank hat keinen Strom. Es riecht unangenehm stechend.')},
'403': {0:('System','„Hauptsicherung“. Eine große Schalteranlage. Jemand hat sie wohl ausgelöst, alle Sicherungen sind herausgesprungen.'),1:('System','Reparaturstecker benutzen?'),
 2:('System','Alle Sicherungen sind verbunden. Jetzt müsste man auch dorthin kommen, wo es bisher nicht ging.'),3:('System','Dank dieser Bodenlampen sieht man unten gerade noch etwas.'),
 4:('System','Das Lampenlicht scheint zeitweise schwächer zu werden…'),5:('Elza','Sherry!'),6:('John','Verdammt! Wo ist sie hin?'),7:('Elza','Teilen wir uns auf und suchen!'),8:('John','OK! Ich gehe hier lang, Elza, du dort!')},
'40A': {0:('System','Da ist ein Schalter… Drücken?'),1:('System','Markierung „Feuer verboten“. Offenbar etwas Gefährliches.'),2:('System','Ein Notfallbett, kaum benutzt.'),3:('System','Streng verriegelt. Zum Öffnen braucht man eine ID-Karte.'),
 4:('System','[ID-Karte des Labors] benutzen?'),5:('System','Eine Art CT-Scanner. Wohl zur Gesundheitskontrolle der Forscher, wurde häufig benutzt.'),6:('System','Ein Regal, innen zu dunkel, um etwas zu erkennen.'),7:('System','Blind herumzutasten wäre gefährlich.'),
 8:('System','Zu dunkel, nichts zu erkennen.'),9:('System','Viel zu dunkel.'),10:('System','Offenbar ein OP-Tisch, aber zu dunkel, um Genaues zu sehen.'),11:('System','Etwas wie Blut klebt daran.')},
'501': {0:('John','Sherry, halt durch!'),1:('John','Nichts zu machen… sie ist bewusstlos.'),2:('Elza','Ach… Sherry.'),3:('Elza','John, in den Akten dieses Labors habe ich etwas Furchtbares gefunden.'),4:('Elza','Der Körper dieses Kindes wird vom Virus zerfressen…'),
 5:('John','Elza, sieh dir das an.'),6:('Elza','Wie furchtbar…'),7:('John','Ohne Impfung wird Sherry…'),8:('Elza','Gut, ich gehe.'),9:('John','Was?'),10:('Elza','Auch jetzt gerade wird Sherrys Körper vom Virus zerfressen.'),
 11:('Elza','Alles oder nichts, ich muss es versuchen.'),12:('John','Elza…'),13:('Elza','John, pass gut auf sie auf.'),14:('Elza','Sherry braucht dich jetzt.'),15:('John','Ich verlasse mich auf dich, Elza.'),16:('John','Dieses Kind will ich unbedingt retten.'),
 17:('Elza','John, der Impfstoff!'),18:('Elza','Was ist mit Sherry?'),19:('John','Schnell! Sie fiebert schon lange.'),20:('Elza','Sherry! Mach die Augen auf!'),21:('John','Sherry! Komm schon, antworte!'),22:('Elza','Bitte… Sherry, wach auf…'),
 23:('Sherry','Was ist denn?'),24:('John','Sherry! Du bist wach!'),25:('Elza','Gott sei Dank…'),26:('Elza','Du hast uns wirklich Sorgen gemacht.'),27:('Elza','John, hast du die Durchsage gehört?'),28:('John','Ja. Was ist hier eigentlich los?'),
 29:('Elza','Das Monster hat die Energieversorgung der Anlage zerstört.'),30:('Elza','Bei der Explosion wird das Virus freigesetzt.'),31:('John','Was!? Dann sind wir auch dran!'),32:('Elza','Genau, hier ist es gefährlich. Schnell zum Schutzraum!'),
 33:('Elza','Sherry, kennst du den Schutzraum?'),34:('Sherry','J-ja.'),35:('Sherry','Schutzraum Nr. 2 kann ich öffnen.'),36:('John','Gut, dann los.'),37:('John','Sherry, ich verlasse mich auf dich.'),
 38:('System','Hier wurde hastig alles durchwühlt. Nichts Brauchbares.'),39:('System','Der Tisch ist staubig. Nichts Besonderes.'),40:('System','Bis vor Kurzem wurde das noch benutzt.'),
 41:('System','Hinter den Geräten hängt eine Tafel mit Notizen. Offenbar Arbeitsvermerke des Labors.'),42:('System','Im Papierkorb liegen achtlos zerknüllte Zettel.'),43:('Elza','Ich muss den Impfstoff schnell holen!')},
'504': {0:('Schild','„Links: P-4-Labor“, „Rechts: Computerraum“')},
'506': {0:('System','Ein Spind mit Schutzanzügen. Offenbar kaputt, er geht nicht auf.'),1:('System','Nichts drin. Sieht aus, als sei er hastig offen gelassen worden.'),2:('System','Zeigt die Biohazard-Stufe an. Im Moment offenbar eine sichere Stufe.'),
 3:('System','Ein Sicherheitsschrank, fest versiegelt.'),4:('System','Die Halteschrauben lösen sich. Hoffentlich ist der Inhalt nicht gefährlich.'),5:('System','Spuren wie Kratzer, wohl von einem Zwischenfall bei der Arbeit.'),
 6:('System','Auch dieses Spülbecken wird sauber gehalten.'),7:('System','Ein Abzug, der bei Gas im Raum anspringt. Ebenfalls sauber, ob er je lief, ist nicht zu erkennen.'),8:('System','Wohl ein Gerät für Virusarbeiten. Nichts Nötiges.'),
 9:('Entwicklervermerk','In dieser ROM ist „Untersuchen“ noch nicht möglich.')},
'50A': {0:('System','Ein Panel für das Rolltor. Offenbar öffnet es nur mit einer besonderen ID-Karte.'),1:('System','Das rechte Rolltor führt offenbar zum Hangar.'),2:('System','Ein Wartungspanel, sieht wenig benutzt aus.'),3:('System','Steht wohl da, damit sich die Forscher nicht verlaufen.')},
'512': {0:('Schild','„Links: P-4-Labor“, „Rechts: Computerraum“'),1:('System','Durch das Feuer verriegelt!')},
'514': {0:('Frauenstimme, nicht zugeordnet','Es ist offen. Da hinten.')},
}
def wo(b, i):
    l, e = f'{b}0:{i}', f'{b}1:{i}'
    a = []
    if l in used: a.append('Leon')
    if e in used: a.append('Elza')
    return ' + '.join(a) if a else 'nicht ausgelöst'
out = ['# Alle Texte der 40%-Disc (Capcom, 6. November 1996)', '',
 'Japanisch direkt aus den RDT-Dateien dekodiert, deutsche Übersetzung und Sprecher von mir aus dem Zusammenhang. Sprecher stehen im japanischen Text nicht dabei.',
 'Spalte „Auslöser“: welches Skript die Zeile wirklich aufruft (`Message_on` oder Untersuchen-AOT). Xxx0 ist Leon, Xxx1 ist Elza. Beide Dateien eines Raums tragen oft denselben Textblock, das sagt nichts über den Spielablauf.',
 'Leerstellen oder falsche Zeichen im Japanischen sind echt: Dort fehlte der Glyph im Font der Disc (`00`-Bytes oder Ersatzzeichen wie 東, 度). In der Übersetzung sind sie aus dem Zusammenhang ergänzt.',
 'Räume ohne Eintrag haben keinen Text; siehe BERICHT.md Abschnitt 8.', '']
n = 0
for b in sorted(D):
    keys = sorted({i for (r, i) in T if r[:3] == b})
    out += [f'## Raum {b}: {ORT.get(b, "")}', '', '| # | Auslöser | Sprecher | Japanisch | Deutsch |', '|---|---|---|---|---|']
    for i in keys:
        t = T.get((b + '0', i)) or T.get((b + '1', i))
        who, de = D[b].get(i, ('?', '?'))
        if de == '?': print('FEHLT', b, i)
        out.append(f'| {i} | {wo(b, i)} | {who} | {t.replace("|", "/")} | {de} |')
        n += 1
    out.append('')
fehlt = sorted({r[:3] for (r, i) in T} - set(D))
print('Zeilen', n, 'Raeume ohne Uebersetzung', fehlt)
open(ziel, 'w', encoding='utf-8').write('\n'.join(out))
