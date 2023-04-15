class star
{
   string name;
   string spectral_type;
   string colour;
   string col;
   int temperature_min;
   int temperature_max;
}

class planet
{
   string name;
   string type;
   string player_name;
   string named_by_player;
   float radius; // In KM
   float mass;   // in 10^24 kg
   string col;
   float gravity; // In Gs, earth=1.
   float surface; // 10^6 km3
   int moons;
}

class starsystem
{
   int *coordinates; // Measure in light years for starsystems.
   string name;
   string player_name;
   string named_by_player;
   class star star;
   int planets_count;
   class planet *planets;
}

class ship_info
{
   string vfile;
   string name;
   string type;
   string starsystem;
   string location;
   string docked_at;
   int long_term;
}

class docking_info
{
   string vfile;
   int docking_time;
   string who;
}