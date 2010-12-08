#include <rvip.h>
#include <OpenSG/OSGSceneFileHandler.h>

OSG_USING_NAMESPACE

int main(int argc, char **argv) {
	gestionnaire_de_scene g(&argc, argv); 
	// g.attacher(makeTorus( .5, 2, 16, 32 ));
	g.attacher(SceneFileHandler::the().read("data/contest.wrl"));
	g.executer(); 
	return 0;
}