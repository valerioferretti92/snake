--Create Match table
CREATE TABLE IF NOT EXISTS Match (
matchId TEXT PRIMARY KEY,
rows INTEGER NOT NULL,
columns INTEGER NOT NULL,
length INTEGER NOT NULL,
sleepTime INTEGER NOT NULL,
gameOver INTEGER NOT NULL DEFAULT 0,
hasWon INTEGER NOT NULL DEFAULT 0,
createdOn TEXT NOT NULL);

--Save match on DB or update its previous record
INSERT INTO Match VALUES("123456789", 30, 30, 20, 20, 1, 1, "123456789");
