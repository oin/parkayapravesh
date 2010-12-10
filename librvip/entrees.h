#if !defined(ENTREES_H_VS2A6YSE)
#define ENTREES_H_VS2A6YSE

#include <ctime>
#include <iostream>

enum evenement_touch_t {
	e_add,			// Le doigt vient d'rentrer
	e_update,		// Le doigt est rentré d'puis un bail
	e_del			// Le doigt sort, moussaillon !
};

// Un doigt
struct doigt {
	doigt(float x, float y, int id, float vitesse = 0, float acceleration = 0) : x_(x), y_(y), vitesse_(vitesse), acceleration_(acceleration), id_(id) {}
	virtual ~doigt() {}
	void x(float nx) { x_ = nx; }
	void y(float ny) { y_ = ny; }
	void vitesse(float nv) { vitesse_ = nv; }
	void acceleration(float na) { acceleration_ = na; }
	float x() const { return x_; }
	float y() const { return y_; }
	float vitesse() const { return vitesse_; }
	float acceleration() const { return acceleration_; }
	int id() const { return id_; }
	bool operator==(doigt& d);
	bool operator!=(doigt& d) { return !operator==(d); }
	bool existe() { return id_ != -1; }
	
	friend std::ostream& operator<<( std::ostream &, doigt &);
	
	static double distance(doigt&, doigt&);
	
	static const doigt aucun;
private:
	doigt(bool) : x_(0), y_(0), id_(-1) {}
	float x_, y_;
	float vitesse_, acceleration_;
	
	int id_;
};

// La situation de l'entrée (doigts + temps associé)
struct luigi {
	typedef std::clock_t temps_t;
	luigi() : doigt1_(doigt::aucun), doigt2_(doigt::aucun), temps_(std::clock()) {}
	luigi(doigt d1) : doigt1_(d1), doigt2_(doigt::aucun), temps_(std::clock()) {}
	luigi(doigt d1, doigt d2) : doigt1_(d1), doigt2_(d2), temps_(std::clock()) {}
	doigt& doigt1() { return doigt1_; }
	doigt& doigt2() { return doigt2_; }
	doigt& premier_doigt_qui_passe();
	int nb_doigts();
	temps_t temps() { return temps_ ; }
	void set_temps() { temps_ = std::clock(); }
	void set_doigt(doigt d);
	void delete_doigt(int id);
	doigt point_median();
	float longueur();
	
	friend std::ostream& operator<<(std::ostream &, luigi &);
private:
	doigt doigt1_, doigt2_;
	std::clock_t temps_;
};

#endif /* end of include guard: ENTREES_H_VS2A6YSE */
