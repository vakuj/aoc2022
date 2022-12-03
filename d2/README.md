# Task day 2

## Summary

From https://adventofcode.com/2022/day/2

---

### Part 1

Rock Paper Scissors is a game between two players. Each game contains many rounds; in each round, the players each simultaneously choose one of Rock, Paper, or Scissors using a hand shape. Then, a winner for that round is selected: 
Rules: 
* Rock defeats Scissors,
* Scissors defeats Paper
* Paper defeats Rock. 
* If both players choose the same shape, the round instead ends in a draw.

Other player selects:
* A for Rock 
* B for Paper
* C for Scissors

Your response: 
* X for Rock 
* Y for Paper
* Z for Scissors

The winner of the whole tournament is the player with the highest score. Your total score is the sum of your scores for each round. The score for a single round is the score for the shape you selected:
* 1 for Rock
* 2 for Paper 
* 3 for Scissors
* plus the score for the outcome of the round:
    * 0 if you lost
    * 3 if the round was a draw
    * 6 if you won

Since you can't be sure if the Elf is trying to help you or trick you, you should calculate the score you would get if you were to follow the strategy guide.

What would your total score be if everything goes exactly according to your strategy guide?

### Example

For example, suppose you were given the following strategy guide:
```
A Y
B X
C Z
```
This strategy guide predicts and recommends the following:

* In the first round, your opponent will choose Rock (A), and you should choose Paper (Y). This ends in a win for you with a score of 8 (2 because you chose Paper + 6 because you won).
* In the second round, your opponent will choose Paper (B), and you should choose Rock (X). This ends in a loss for you with a score of 1 (1 + 0).
* The third round is a draw with both players choosing Scissors, giving you a score of 3 + 3 = 6.

In this example, if you were to follow the strategy guide, you would get a total score of 15 (8 + 1 + 6).


---

### Part 2


* X = lose
* Y = draw
* Z = win

The total score is still calculated in the same way, but now you need to figure out what shape to choose so the round ends as indicated. 

Following the Elf's instructions for the second column, what would your total score be if everything goes exactly according to your strategy guide?

### Example

The example above now goes like this:

* In the first round, your opponent will choose Rock (A), and you need the round to end in a draw (Y), so you also choose Rock. This gives you a score of 1 + 3 = 4.
* In the second round, your opponent will choose Paper (B), and you choose Rock so you lose (X) with a score of 1 + 0 = 1.
* In the third round, you will defeat your opponent's Scissors with Rock for a score of 1 + 6 = 7.

Now that you're correctly decrypting the ultra top secret strategy guide, you would get a total score of 12.

___


