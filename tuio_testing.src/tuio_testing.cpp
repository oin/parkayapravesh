#include <rvip.h>
#include <OpenSG/OSGSceneFileHandler.h>
#include <mario.h>
#include <peach.h>

OSG_USING_NAMESPACE

int main(int argc, char **argv) {
	gestionnaire_de_scene g(&argc, argv); 
	// g.attacher(makeTorus( .5, 2, 16, 32 ));
	//g.attacher(SceneFileHandler::the().read("data/contest.wrl"));
	peach tuiote_moi_ca;
	std::cout << CLOCKS_PER_SEC << std::endl;
	//g.executer(); 
	std::clock_t a = std::clock();
	while(true) {
		if((std::clock() - a) * 1.0 / CLOCKS_PER_SEC > 0.5 ) {
			tuiote_moi_ca.mettre_a_jour();
			a = std::clock();
		}
		
	}	
	return 0;
}