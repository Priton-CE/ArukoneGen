## Note
This project is part of my attempt in the "Bundeswettbewerb Informatik 42" competition. The goal was to generate a valid arukone puzzle that could not be solved by a provided solver program. My goal was to keep the program simple, lightweight, and not to overload it with functionality.
The code is now being published here as I think it turned out quite nicely.

# Arukone Generator
Arukone is a game where you connect two points using a line that cannot cross other lines.
This project generates a puzzle of variable sizes.

## Usage
```./arukone <puzzle size>```
Returns multiple number of lines:
1. The seed used (may repeat if there was no solution for the given seed and a new one was used instead)
2. The size of the puzzle
3. The number of pairs to connect
4. The actual field with a `0` being empty space and higher being a pair.

## Example
```
./arukone 5

SEED: 1708026158
5
3
0 0 0 0 0
3 0 3 2 0
0 0 0 0 0
1 2 0 0 0
0 0 0 0 1
```

# Original German Documentation
### Lösungsidee

Es werden die Anfangspunkte der Zahlenpaare zufällig auf dem Feld generiert. Es wird dann zufällig die Anzahl der maximalen Segmente für jedes Zahlenpaar erzeugt. (Ein Segment ist ein Teil der finalen Verbindungslinie.)

Von den Punkten ausgehend wird dann in eine zufällige Richtung eine Linie generiert, bis diese auf ein Hinderniss trifft. Von dort geht sie in erneut in eine zufällige Richtung bis sie auf ein Hinderniss trifft, wo der Prozess wiederholt wird, sie in einer Sackgasse endet oder bis die maximalen Elemente erreicht wurden, wo bei beiden der Prozess zum nächsten Zahlenpaar wechselt. Wechselt der Prozess wird an dem letzten Element der Linie der Endpunkt eingefügt.

Ist irgend ein Punkt so eingeschlossen, dass keine Linie generiert werden kann muss der gesamte Prozess neu gestartet werden.

### Umsetzung

Als "Infrastruktur" wurde eine Struktur erstellt, die die Details eines Arukone Puzzel enthält (Feld, Dimensionen und Zahlenpaare). Es gibt zwei Funktionen, die diese Struktur erzeugen und zerstören und eine weitere, die das Puzzel generiert. Als Hilfe gibt es auch noch eine Vektor Struktur, um 2-Dimensionale Vektoren zu halten.

Es gibt zwei Helferfunktionen. Eine, um Zufallszahlen zwischen 0 und einer oberen Grenze zu generieren, und eine zweite, um das Feld zu debugging Gründen in die Konsole zu schreiben. (Alle Debug Nachrichten können durch das Einkommentieren des `DEBUG_PRINT` über dieser Funktion aktiviert werden.)

Zum generieren der Startpunkte und der gewünschten Segmentanzahlen gibt es zwei Schleifen, die zwei auf dem HEAP erzeugte Listen befüllen. Zur generation der Pfade gibt es zwei ineinander verschachtelte Schleifen. Die äußere durchläuft alle Anfangspositionen der Zahlenpaare und überprüft, ob die Startpositionen frei sind, und die innere, die da darauf folgt, generiert den Pfad, schrittweise, entsprechend des Lösungsansatzes.

Zum Übergeben von blockierten Richtungen wird der `char` Datentyp verwendet, der über Bitmanipulation eine Liste mit markierten Richtungen enthält (für Details siehe `arukone.c`).

### Beispiele

Aus 10 Versuchen wurden 4 Puzzle generiert, die vom bereitgestellten Solver nicht gelößt werden konnten:

```
SEED: 1694535161
10
8
0 0 7 0 0 0 0 0 0 5
0 0 6 0 0 0 0 0 0 8
0 7 0 0 0 0 0 0 0 0
6 0 0 0 0 0 0 0 0 0
0 0 0 0 0 1 0 0 0 0
0 5 0 0 0 0 0 0 0 8
0 0 0 0 0 0 0 0 3 2
0 0 4 0 0 0 0 0 4 0
0 0 0 0 0 3 2 0 0 0
0 0 0 0 0 0 0 0 0 1
```

```
SEED: 1694535373
10
8
0 0 0 0 5 0 0 0 0 8
0 0 0 0 0 0 0 0 1 0
0 3 0 0 0 0 0 0 6 0
0 0 5 0 0 0 0 0 0 0
0 0 2 0 0 0 0 8 0 0
0 0 0 3 0 1 0 0 0 6
0 0 0 0 0 0 0 0 0 4
0 0 0 0 0 0 7 0 0 0
0 0 0 0 0 0 7 0 0 0
2 0 0 0 0 0 0 0 4 0
```

```
SEED: 788638929
10
8
2 0 0 0 0 0 0 0 0 1
0 0 1 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
0 0 0 0 8 0 0 0 0 0
0 0 4 0 0 0 7 0 0 4
0 0 0 0 0 6 0 0 0 5
0 8 0 0 0 7 0 0 0 0
0 0 0 0 0 0 0 3 0 0
0 3 0 0 0 0 0 0 6 0
0 0 0 0 0 0 0 0 2 5
```

```
SEED: 1694535482
10
8
1 0 0 0 0 0 0 0 0 4
0 0 5 0 0 0 0 0 0 8
0 0 7 0 0 0 0 0 0 8
0 0 0 0 4 0 0 0 0 2
0 0 0 0 0 1 0 0 0 0
0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0
6 0 0 0 6 3 2 7 5 0
3 0 0 0 0 0 0 0 0 0
```
