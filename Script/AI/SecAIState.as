
enum ESecAIState
{
	// Walking around randomly or along a patrol path with no particular care
	Idle,
	// Has been notified of something, such as a possible target in the distance or a sound close by
	Alert,
	// Engaged in combat with a target
	Engaged
}
