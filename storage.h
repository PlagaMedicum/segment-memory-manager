typedef char** SEGMENT;

struct segment_table
{
	bool is_present;	// Checks if segment is present in memory
	char* beginning;	// points to beggining of segment
	int length;
};

struct segment_link
{
	int d; // Adress shift.
	int s; // Segment index.
};

