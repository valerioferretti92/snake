--Create Match table
CREATE TABLE Match (
matchId INTEGER PRIMARY KEY,
rows INTEGER NOT NULL,
columns INTEGER NOT NULL,
appleX INTEGER NOT NULL,
appleY INTEGER NOT NULL,
cDirection TEXT NOT NULL,
sleepTime INTEGER NOT NULL,
gameOver INTEGER NOT NULL,
hasWon INTEGER NOT NULL DEFAULT 0);

--Create Snake table
CREATE TABLE Snake (
x INTEGER NOT NULL,
y INTEGER NOT NULL,
matchId INTEGER NOT NULL,
sequenceNumber INTEGER NOT NULL,
PRIMARY KEY (x, y, matchId),
FOREIGN KEY(matchId) REFERENCES Match(matchId) ON DELETE CASCADE;

--Enforcing referencial integrity checks in sqlite3
PRAGMA foreign_keys = ON;

--Checking exisitance of open matches
SELECT COUNT(*) FROM Match WHERE gameOver = 0;
--Retrieve latest open match
SELECT * FROM Match WHERE gameOver = 0 ORDER BY matchId DESC LIMIT 1;
--Retrieve snake points of latest open match
SELECT * FROM Snake WHERE matchId = 123456789 ORDER BY sequenceNumber DESC;
--Mark all other matches as terminated
UPDATE Match SET gameOver = 1 WHERE gameOver = 0;
--Save match on DB or update its previous record
INSERT INTO Match VALUES(123456789, 30, 30, 20, 20, 'w', 50, 1, 1) ON CONFLICT(matchId) DO
UPDATE SET rows = 30, columns = 30, appleX = 20, appleY = 20, cDirection = 'w', sleepTime = 50, gameOver = 1,hasWon = 1;
--Delete snake points on DB
DELETE FROM Snake WHERE matchId = 123456789;
--Save snake on DB
INSERT INTO Snake VALUES (12, 14, 123456789, 2), (12, 13, 123456789, 1), (12, 12, 123456789, 0);
