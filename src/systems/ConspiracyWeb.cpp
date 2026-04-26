#include "ConspiracyWeb.h"

#include <fstream>

#include <cmath>

#include <algorithm>

#include <cstdlib>



using json = nlohmann::json;



namespace systems {



    ConspiracyWeb::ConspiracyWeb() {}



    void ConspiracyWeb::addNode(const std::string& id, const std::string& title, const std::string& type,

                                 const std::vector<std::string>& connections, float paranoia, float awakening, const std::string& content) {

        ConspiracyNode node;

        node.id = id;

        node.title = title;

        node.type = type;

        node.connections = connections;

        node.paranoia_on_read = paranoia;

        node.awakening_on_read = awakening;
        node.content = content;
        node.discovered = true; // Force discovered for hardcoded events to ensure visibility

        // Random position within board space (X: 50-170, Y: 100-280)
        node.x = 60.0f + static_cast<float>(std::rand() % 100);
        node.y = 110.0f + static_cast<float>(std::rand() % 160);

        node.vx = node.vy = 0;

        nodes_[id] = node;

    }



    void ConspiracyWeb::loadHardcodedFallback() {
        std::srand(42);
        nodes_.clear();
        addNode("dacian_protochronism", "Protochronismul Dacic", "IDEOLOGY", {"tartaria_tablets", "zalmoxis"}, 0.11f, 0.14f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("burebista_empire", "Imperiul lui Burebista", "SYMBOL", {"dacian_realm", "dacian_protochronism"}, 0.07f, 0.09f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("decebal_treasure", "Comoara lui Decebal", "ARTIFACT", {"casa_tunnels", "immortality_ritual"}, 0.08f, 0.12f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("sarmizegetusa_portal", "Sarmizegetusa - Poarta Umbrei", "LOCATION", {"dacian_realm", "hoia_baciu"}, 0.09f, 0.16f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("pelasgian_theory", "Teoria Pelasgilor", "IDEOLOGY", {"tartaria_tablets", "dacian_protochronism"}, 0.06f, 0.08f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("dacians_invented_writing", "Dacii au Inventat Scrierea", "DOCUMENT", {"tartaria_tablets"}, 0.12f, 0.10f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("dacian_lasers", "Lasere Dacice - Arma Secreta", "ARTIFACT", {"revolution_1989", "strigoi_network"}, 0.05f, 0.07f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("ceausescu_dacomania", "Dacomania lui Ceaușescu", "DOCUMENT", {"dacian_protochronism", "revolution_1989"}, 0.08f, 0.06f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("dacii_si_lupii", "Dacii și Lupii - Simbolul Străvechi", "SYMBOL", {"pricolici", "zalmoxis"}, 0.07f, 0.11f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("vinca_danube_script", "Scriptul Dunărean Vinča", "ARTIFACT", {"tartaria_tablets"}, 0.10f, 0.13f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("moroi_spirits", "Moroi - Spiritele Necurate", "ENTITY", {"strigoi_network", "immortality_ritual"}, 0.13f, 0.15f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("pricolici_werewolves", "Pricolici - Lupii Nemuritori", "ENTITY", {"strigoi_network", "dacian_realm"}, 0.11f, 0.14f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("iele_dancers", "Iele - Dansatoarele Lunii", "ENTITY", {"hoia_baciu", "dacian_realm"}, 0.09f, 0.17f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("mama_padurii", "Mama Pădurii - Stăpâna Codrului", "ENTITY", {"hoia_baciu"}, 0.08f, 0.12f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("varcolaci_eclipse", "Vârcolaci - Mâncătorii de Lună", "ENTITY", {"pricolici", "zalmoxis"}, 0.07f, 0.10f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("strigoi_vii", "Strigoi Vii - Cei vii care sug viață", "ENTITY", {"strigoi_network", "moroi_spirits"}, 0.14f, 0.16f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("strigoi_morti", "Strigoi Morți - Morții care se întorc", "ENTITY", {"immortality_ritual"}, 0.12f, 0.13f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("ursitoare_fates", "Ursitoare - Cele Trei Surori", "ENTITY", {"zalmoxis", "immortality_ritual"}, 0.06f, 0.09f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("baba_cloanta", "Baba Cloanța - Vrăjitoarea", "ENTITY", {"iele_dancers", "moroi_spirits"}, 0.05f, 0.08f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("capcaun_ogre", "Căpcăun - Devoratorul de Oameni", "ENTITY", {"dacian_realm"}, 0.04f, 0.07f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("shadow_dacian_priest", "Preot Dacic din Umbră", "ENTITY", {"zalmoxis", "dacian_realm"}, 0.10f, 0.15f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("energy_drain_ritual", "Ritualul de Scurgere a Energiei", "ARTIFACT", {"strigoi_network", "moroi_spirits"}, 0.09f, 0.14f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("hoia_baciu", "Pădurea Hoia-Baciu - Poarta Bermudei Transilvane", "LOCATION", {"dacian_realm", "iele_dancers"}, 0.15f, 0.20f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("bran_castle_tunnels", "Tunelurile Castelului Bran", "LOCATION", {"casa_tunnels", "strigoi_network"}, 0.08f, 0.11f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("salt_mines_turda", "Salina Turda - Labirint Subteran", "LOCATION", {"casa_tunnels", "dacian_realm"}, 0.07f, 0.09f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("bucegi_sphinx", "Sfinxul din Bucegi - Portal Energetic", "LOCATION", {"dacian_realm", "zalmoxis"}, 0.12f, 0.18f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("carpathian_giants", "Giganții Carpați", "ENTITY", {"hoia_baciu", "pricolici"}, 0.06f, 0.10f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("underground_dacian_city", "Orașul Dacic Subteran", "LOCATION", {"casa_tunnels", "sarmizegetusa_portal"}, 0.09f, 0.13f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("poiana_rotunda", "Poiana Rotundă - Cercurile Fără Viață", "LOCATION", {"hoia_baciu"}, 0.11f, 0.16f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("transfagarasan_anomalies", "Anomalii pe Transfăgărășan", "LOCATION", {"hoia_baciu", "dacian_lasers"}, 0.05f, 0.08f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("bucharest_deep_web_forum", "Zalmoxis.net - Forumul Ascuns", "DOCUMENT", {"strigoi_network", "revolution_1989"}, 0.10f, 0.07f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("securitate_archives", "Arhivele Securității - Fișierele Umbrei", "DOCUMENT", {"revolution_1989", "ceausescu_dacomania"}, 0.08f, 0.06f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("sri_strigoi_infiltration", "Infiltrarea SRI cu Strigoi", "DOCUMENT", {"strigoi_network", "revolution_1989"}, 0.13f, 0.09f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("eu_suppression_dacia", "Suprimarea Daciei de către UE", "DOCUMENT", {"eu_pact", "dacian_protochronism"}, 0.07f, 0.05f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("tiktok_dacian_awakening", "Trezirea Dacică pe TikTok", "DOCUMENT", {"dacian_protochronism", "strigoi_network"}, 0.09f, 0.12f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("influencer_iele", "Influenceri Iele - Scurgători de Energie", "ENTITY", {"iele_dancers", "energy_drain_ritual"}, 0.06f, 0.11f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("neo_dacian_politician", "Politicianul Neo-Dacic", "ENTITY", {"dacian_protochronism", "strigoi_network"}, 0.08f, 0.10f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("alien_dacian_tech", "Tehnologie Dacică Extraterestră", "ARTIFACT", {"dacian_lasers", "hoia_baciu"}, 0.05f, 0.14f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("1989_coup_coverup", "Lovitura de Stat 1989 - Acoperirea Tunelurilor", "DOCUMENT", {"revolution_1989", "casa_tunnels"}, 0.10f, 0.08f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("vatican_artifact_trade", "Comerțul cu Artefacte Dacice de către Vatican", "DOCUMENT", {"eu_pact"}, 0.06f, 0.04f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("romanian_deep_state", "Deep State-ul Românesc", "ENTITY", {"sri_strigoi_infiltration", "strigoi_network"}, 0.12f, 0.09f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("glitch_reality_events", "Evenimente de Glitch al Realității", "SYMBOL", {"dacian_realm", "hoia_baciu"}, 0.11f, 0.17f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("orthodox_anti_strigoi", "Ritualuri Ortodoxe Anti-Strigoi", "ARTIFACT", {"immortality_ritual", "strigoi_morti"}, 0.07f, 0.06f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("ceausescu_immortality", "Ceaușescu și Căutarea Nemuririi", "DOCUMENT", {"ceausescu_dacomania", "immortality_ritual"}, 0.05f, 0.07f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("dacian_revival_society", "Societatea Renașterii Dacice", "ENTITY", {"dacian_protochronism", "neo_dacian_politician"}, 0.08f, 0.11f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("you_are_the_strigoi", "Tu Ești Strigoiul Uitat", "ENTITY", {"strigoi_network", "immortality_ritual"}, 0.15f, 0.20f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("zalmoxis_awakening_2025", "Trezirea lui Zalmoxis - 2025", "SYMBOL", {"zalmoxis", "dacian_realm"}, 0.14f, 0.19f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("mirror_portal_ritual", "Ritualul Prin Oglindă", "ARTIFACT", {"strigoi_vii", "hoia_baciu"}, 0.10f, 0.15f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("global_dacian_pact", "Pactul Global Dacic", "DOCUMENT", {"eu_pact", "vatican_artifact_trade"}, 0.06f, 0.05f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");


addNode("final_revelation_loop", "Bucla Revelației Finale", "SYMBOL", {"dacian_realm", "you_are_the_strigoi"}, 0.16f, 0.22f, "Toate rapoartele clasificate despre Protochronismul Dacic au dispărut simultan.\nDoar cei ce au ochi de văzut pot observa anomaliile zilnice.\nAdevărul s-a ascuns la vedere de decenii întregi.\nUrmele sunt impregnate adânc în energiile subtile ale munților.\nCâteva pagini au supraviețuit dintr-un jurnal găsit în arhive.");
        nodePtrs_.clear();

        for (auto& p : nodes_) nodePtrs_.push_back(&p.second);

        runForceSimulation();

    }



    void ConspiracyWeb::loadFromFile(const std::string& path) {

        std::ifstream file(path);

        if (!file.is_open()) {

            // Fallback to hardcoded nodes

            loadHardcodedFallback();

            return;

        }



        json data;

        try {

            file >> data;

            file.close();

        } catch (...) {

            loadHardcodedFallback();

            return;

        }



        if (data.contains("nodes")) {

            loadNodes(data["nodes"]);

        } else {

            loadHardcodedFallback();

        }

    }



    void ConspiracyWeb::loadNodes(const json& data) {

        if (!data.is_array()) {

            loadHardcodedFallback();

            return;

        }



        int idx = 0;

        for (const auto& nodeData : data) {

            if (!nodeData.contains("id")) continue;

            ConspiracyNode node;

            node.id = nodeData["id"];

            node.title = nodeData.value("title", "");

            node.content = nodeData.value("content", "");

            node.type = nodeData.value("type", "DOCUMENT");



            float angle = (idx * 2.0f * 3.14159f) / std::max(1, (int)data.size() - 1);

            float radius = 60.0f;

            node.x = 110.0f + radius * std::cos(angle);
            node.y = 190.0f + radius * std::sin(angle);

            node.vx = node.vy = 0;



            if (nodeData.contains("connections") && nodeData["connections"].is_array()) {

                for (const auto& c : nodeData["connections"]) node.connections.push_back(c.get<std::string>());

            }

            node.paranoia_on_read = nodeData.value("paranoia_on_read", 0.05f);

            node.awakening_on_read = nodeData.value("awakening_on_read", 0.02f);



            nodes_[node.id] = node;

            idx++;

        }



        nodePtrs_.clear();

        for (auto& p : nodes_) nodePtrs_.push_back(&p.second);

        runForceSimulation();

    }



    void ConspiracyWeb::runForceSimulation() {

        const int iterations = 50;

        const float repulsion = 500.0f;

        const float attraction = 0.02f;

        const float damping = 0.85f;



        for (int iter = 0; iter < iterations; ++iter) {

            for (auto* A : nodePtrs_) {

                A->vx = A->vy = 0;

                // Center attraction

                float cx = 110.0f - A->x;
                float cy = 190.0f - A->y;

                A->vx += cx * 0.002f;

                A->vy += cy * 0.002f;



                for (auto* B : nodePtrs_) {

                    if (A == B) continue;

                    float dx = A->x - B->x, dy = A->y - B->y;

                    float dist = std::sqrt(dx*dx + dy*dy);

                    if (dist < 1.0f) dist = 1.0f;

                    float f = repulsion / (dist * dist);

                    A->vx += (dx/dist) * f;

                    A->vy += (dy/dist) * f;

                }

                for (const auto& cid : A->connections) {

                    auto it = nodes_.find(cid);

                    if (it == nodes_.end()) continue;

                    ConspiracyNode* B = &it->second;

                    float dx = B->x - A->x, dy = B->y - A->y;

                    float dist = std::sqrt(dx*dx + dy*dy);

                    if (dist < 1.0f) dist = 1.0f;

                    A->vx += (dx/dist) * dist * attraction;

                    A->vy += (dy/dist) * dist * attraction;

                }

            }

            for (auto* n : nodePtrs_) {

                n->x += n->vx * damping;

                n->y += n->vy * damping;

                n->x = std::max(55.0f, std::min(165.0f, n->x));
                n->y = std::max(105.0f, std::min(275.0f, n->y));

            }

        }

    }



    void ConspiracyWeb::discoverNode(const std::string& id) {

        auto it = nodes_.find(id);

        if (it != nodes_.end()) it->second.discovered = true;

    }



    bool ConspiracyWeb::isDiscovered(const std::string& id) const {

        auto it = nodes_.find(id);

        return it != nodes_.end() && it->second.discovered;

    }



    const ConspiracyNode* ConspiracyWeb::getNode(const std::string& id) const {

        auto it = nodes_.find(id);

        return it != nodes_.end() ? &it->second : nullptr;

    }



    void ConspiracyWeb::updatePhysics() {

        // Run 10 force iterations per frame for live layout

        const float repulsion = 500.0f;

        const float attraction = 0.02f;

        const float damping = 0.85f;



        for (int iter = 0; iter < 10; ++iter) {

            for (auto* A : nodePtrs_) {

                float fx = 0, fy = 0;

                // Center attraction

                fx += (110.0f - A->x) * 0.002f;
                fy += (190.0f - A->y) * 0.002f;



                for (auto* B : nodePtrs_) {

                    if (A == B) continue;

                    float dx = A->x - B->x, dy = A->y - B->y;

                    float dist = std::sqrt(dx*dx + dy*dy);

                    if (dist < 1.0f) dist = 1.0f;

                    float f = repulsion / (dist * dist);

                    fx += (dx/dist) * f;

                    fy += (dy/dist) * f;

                }

                for (const auto& cid : A->connections) {

                    auto it = nodes_.find(cid);

                    if (it == nodes_.end()) continue;

                    ConspiracyNode* B = &it->second;

                    float dx = B->x - A->x, dy = B->y - A->y;

                    float dist = std::sqrt(dx*dx + dy*dy);

                    if (dist < 1.0f) dist = 1.0f;

                    fx += (dx/dist) * dist * attraction;

                    fy += (dy/dist) * dist * attraction;

                }

                A->vx = (A->vx + fx) * damping;

                A->vy = (A->vy + fy) * damping;

            }

            for (auto* n : nodePtrs_) {

                n->x += n->vx;

                n->y += n->vy;

                n->x = std::max(55.0f, std::min(165.0f, n->x));
                n->y = std::max(105.0f, std::min(275.0f, n->y));

            }

        }

    }



    void ConspiracyWeb::updatePositions() { updatePhysics(); }



    int ConspiracyWeb::getDiscoveredCount() const {

        int c = 0;

        for (auto& p : nodes_) if (p.second.discovered) c++;

        return c;

    }



    float ConspiracyWeb::getDiscoveryRatio() const {

        return nodes_.empty() ? 0.0f : (float)getDiscoveredCount() / static_cast<float>(nodes_.size());

    }

}

