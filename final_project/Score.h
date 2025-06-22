/**
 * File Name: Score.h
 * Author: Alexandre Kévin DE FREITAS MARTINS
 * Creation Date: 22/6/2025
 * Description: This is the Score.h
 * Copyright (c) 2025 Alexandre Kévin DE FREITAS MARTINS
 * Version: 1.0.0
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the 'Software'), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef SCORE_H
#define SCORE_H

class Score {
public:
    Score() = default;
    ~Score() = default;
    Score(const Score &other) = default;
    Score(int score) : score(score) {};
    Score &operator=(const Score &other) = default;
    int getScore() const { return score; }
    std::string getScoreString() const { return std::to_string(score); }
    void setScore(const int newScore) { score = newScore; }
    void addToScore(const int additionalScore) { score += additionalScore; }
    void removeToScore(const int subtractedScore) { score -= subtractedScore; }

private:
    int score = 0;
};

#endif // SCORE_H
