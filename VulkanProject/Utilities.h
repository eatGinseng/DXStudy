#pragma once
// indivces of queue families (if they exists at all)
struct QueueFamilyIndices {

	int GraphicsFamily = -1;	// Location Graphics Queue Family


	// check if queue families are valid
	bool IsValid()
	{
		return GraphicsFamily >= 0;
	}
};