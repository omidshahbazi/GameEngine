float Map(float Value, float OriginalMin, float OriginalMax, float NewMin, float NewMax, float Scale)
{
	return (((((Value - OriginalMin) / (OriginalMax - OriginalMin)) * Scale) % 1) * (NewMax - NewMin)) + NewMin;
}

#define Omid