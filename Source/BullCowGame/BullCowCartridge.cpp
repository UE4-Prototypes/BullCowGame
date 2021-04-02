// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
#include <Misc/FileHelper.h>
#include <Misc/Paths.h>

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();

    {
        TArray<FString> Words;
        const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordList/HiddenWordList.txt");
        FFileHelper::LoadFileToStringArray(Words, *WordListPath);
        Isograms = GetValidWords(Words);
    }

    SetupGame();
}

void UBullCowCartridge::OnInput(const FString& Input) // When the player hits enter
{
    ClearScreen();   
    if (bGameOver)
    {
        ClearScreen();
        if (PlayAgain(Input))
        {
            SetupGame();
        }
    }
    else
    {
        ProcessGuess(Input);
    }
    PrintLine(TEXT("Guess the %i letter word!"), HiddenWord.Len());
}

void UBullCowCartridge::SetupGame()
{
    // Welcome player
    PrintLine(TEXT("Welcome to Bulls Cows!"));

    HiddenWord = Isograms[FMath::RandRange(0, (Isograms.Num() - 1))];
    Lives = HiddenWord.Len() * 2;
    bGameOver = false;

    PrintLine(TEXT("The HiddenWord is: %s."), *HiddenWord); // Debug Line
    PrintLine(TEXT("Guess the %i letter word!"), HiddenWord.Len());
    PrintLine(TEXT("You have %i chances"), Lives);
    PrintLine(TEXT("Type in your guess. \nPress enter to continue..."));

    const TCHAR HW[] = TEXT("above");
}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;
    PrintLine(TEXT("\nPress enter to continue..."));
}

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
    if (Guess == HiddenWord)
    {
        ClearScreen();
        PrintLine(TEXT("You guessed correctly and Won!"));
        EndGame();
        return;
    }

    if (Guess.Len() != HiddenWord.Len())
    {
        PrintLine(TEXT("The hidden word is %i letters long."), HiddenWord.Len());
        PrintLine(TEXT("Guess again, you have %i lives remaining."), Lives);
        return;
    }

    if (!IsIsogram(Guess))
    {
        PrintLine(TEXT("No repeating letters, guess again..."));
        return;
    }

    --Lives;
    PrintLine(TEXT("You have lost a life!"));

    if (Lives <= 0)
    {
        ClearScreen();
        PrintLine(TEXT("You have no lives left!"));
        PrintLine(TEXT("The hidden word was: %s"), *HiddenWord);
        EndGame();
        return;
    }
    
    // Show the player Bulls and Cows
    FBullCowCount Count = GetBullCows(Guess);
    
    PrintLine(TEXT("You have %i Bulls and %i Cows"), Count.Bulls, Count.Cows);

    PrintLine(TEXT("\nGuess again, you have %i lives remaining."), Lives);
}

bool UBullCowCartridge::IsIsogram(const FString& Word) const
{
    for (int32 Index = 0; Index < Word.Len(); Index++)
    {
        for (int32 Comparison = Index + 1; Comparison < Word.Len(); Comparison++)
        {
            if (Word[Index] == Word[Comparison])
            {
                return false;
            }
        }
    }

    return true;
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& WordList) const
{
    TArray<FString> ValidWords;

    for (FString Word : WordList)
    {
        if (Word.Len() >= 4 && Word.Len() <= 7)
        {
            ValidWords.Emplace(Word);
        }
    }
    return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const
{
    FBullCowCount Count;
    for (int32 GuessIndex = 0; GuessIndex < Guess.Len(); GuessIndex++)
    {
        if (Guess[GuessIndex] == HiddenWord[GuessIndex])
        {
            Count.Bulls++;
            continue;
        }

        for (int32 HiddenIndex = 0; HiddenIndex < HiddenWord.Len(); HiddenIndex++)
        {
            if (Guess[GuessIndex] == HiddenWord[HiddenIndex])
            {
                Count.Cows++;
                break;
            }
        }
    }
    return Count;
}

bool UBullCowCartridge::PlayAgain(const FString& Choice) const
{
    PrintLine(TEXT("Do you want to play again (yes/no)?"));

    if (Choice == TEXT("yes"))
    {
        return true;
    }
    return false;
}