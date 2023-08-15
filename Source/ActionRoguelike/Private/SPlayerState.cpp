// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

int ASPlayerState::GetCredits() const {
	return Credits;
}

void ASPlayerState::AddCredits(int32 Delta) {
	if (!ensure(Delta > 0.0f)) {
		return;
	}

	Credits += Delta;

	OnCreditsChanged.Broadcast(this, Credits, Delta);
}

bool ASPlayerState::RemoveCredits(int32 Delta) {
	if (!ensure(Delta > 0.0f)) {
		return false;
	}

	if (Delta > Credits) {
		return false;
	}

	Credits -= Delta;

	// It needs to be distinguished from AddCredits.
	OnCreditsChanged.Broadcast(this, Credits, -Delta);

	return true;
}

