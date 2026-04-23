#include "main.h"


unsigned char* loadBMPFile(const char* filename, int* width, int* height)
{
	const int HEADERSIZE = 54;
	int i, j;
	int real_width;
	unsigned int color;
	FILE* fp;
	unsigned char header_buf[HEADERSIZE];
	unsigned char* tmp;
	unsigned char* pixel;
	errno_t err = fopen_s(&fp, filename, "rb");
	if (err != 0) {
		fprintf(stderr, "Cannot open file [ %s ].\n", filename);
		exit(-1);
	}

	fread(header_buf, sizeof(unsigned char), HEADERSIZE, fp);

	if (strncmp((char*)header_buf, "BM", 2)) {
		fprintf(stderr, "Error : [ %s ] is not bitmap file.\n", filename);
		exit(-1);
	}
	memcpy(width, header_buf + 18, sizeof(int));
	memcpy(height, header_buf + 22, sizeof(int));
	memcpy(&color, header_buf + 28, sizeof(unsigned int));

	if (color != 24) {
		fprintf(stderr, "Error : [ %s ] is not 24bit color image.\n", filename);
		exit(-1);
	}

	if (*height < 0) {
		*height *= -1;
		real_width = *width * 3 + *width % 4;
		tmp = new unsigned char[real_width];
		pixel = new unsigned char[4 * (*width) * (*height)];

		for (i = 0; i < *height; i++) {
			fread(tmp, 1, real_width, fp);
			for (j = 0; j < *width; j++) {
				pixel[4 * ((*height - i - 1) * (*width) + j)] = tmp[3 * j + 2];
				pixel[4 * ((*height - i - 1) * (*width) + j) + 1] = tmp[3 * j + 1];
				pixel[4 * ((*height - i - 1) * (*width) + j) + 2] = tmp[3 * j];
				pixel[4 * ((*height - i - 1) * (*width) + j) + 3] = 255;
				if (tmp[3 * j] < 5 && tmp[3 * j + 1] < 5 && tmp[3 * j + 2] < 5) {
					pixel[4 * ((*height - i - 1) * (*width) + j)] = tmp[3 * j + 2];
					pixel[4 * ((*height - i - 1) * (*width) + j) + 1] = tmp[3 * j + 1];
					pixel[4 * ((*height - i - 1) * (*width) + j) + 2] = tmp[3 * j];
					pixel[4 * ((*height - i - 1) * (*width) + j) + 3] = 0;
				}
			}
		}
	}
	else {
		real_width = *width * 3 + *width % 4;
		tmp = new unsigned char[real_width];
		pixel = new unsigned char[4 * (*width) * (*height)];

		for (i = 0; i < *height; i++) {
			fread(tmp, 1, real_width, fp);
			for (j = 0; j < *width; j++) {
				pixel[4 * (i * (*width) + j)] = tmp[3 * j + 2];
				pixel[4 * (i * (*width) + j) + 1] = tmp[3 * j + 1];
				pixel[4 * (i * (*width) + j) + 2] = tmp[3 * j];
				pixel[4 * (i * (*width) + j) + 3] = 255;
				if (tmp[3 * j] <5 && tmp[3 * j + 1] < 5 && tmp[3 * j+2] < 5) {
					pixel[4 * (i * (*width) + j)] = tmp[3 * j + 2];
					pixel[4 * (i * (*width) + j) + 1] = tmp[3 * j + 1];
					pixel[4 * (i * (*width) + j) + 2] = tmp[3 * j];
					pixel[4 * (i * (*width) + j) + 3] = 0;
				}
			}
		}
	}
	delete[] tmp;
	return pixel;
}

GLuint loadTexture(const char* filename)
{
	GLuint texID;
	GLubyte* texture_image;
	int texture_width, texture_height;
	texture_image = loadBMPFile(filename, &texture_width, &texture_height);
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_image);
	delete[] texture_image;
	return texID;
}

void displayTextureplate() {
	glColor3f(1.f, 1.f, 1.f);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glBegin(GL_QUADS);
	//glNormal3d(normal[0], normal[1], normal[2]);
	glNormal3f(0.f, 0.f, 1.f);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 0.f);
	glVertex2f(-5.f, -5.f);
	glTexCoord2f(1.f, 0.f);
	glVertex2f(5.f, -5.f);
	glTexCoord2f(1.f, 1.f);
	glVertex2f(5.f, 5.f);
	glTexCoord2f(0.f, 1.f);
	glVertex2f(-5.f, 5.f);
	glEnd();
}


void setCamera(void) {
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30.f, (GLdouble)WINDOW_WIDTH / (GLdouble)WINDOW_HEIGHT, 0.1f, 1000.f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		stats_self[POSIT][0]+ stats_self[HEADI][0]*10, stats_self[POSIT][1] + stats_self[HEADI][1] * 10, stats_self[POSIT][2] + stats_self[HEADI][2] * 10,
		stats_self[POSIT][0], stats_self[POSIT][1], stats_self[POSIT][2], 
		stats_self[TOP][0], stats_self[TOP][1], stats_self[TOP][2]
	);
}

void setLight(void)
{
	GLfloat light_position[4] = { 5.f, 25.f, 5.f, 1.f };
	GLfloat light_ambient[4] = { 0.3f, 0.3f, 0.3f, 0.f };
	GLfloat light_diffuse[4] = { 1.f, 1.f, 1.f, 1.f };
	GLfloat light_specular[4] = { 1.f, 1.f, 1.f, 1.f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
}

void displaySphere(float radius, float dr, float dg, float db, float sr, float sg, float sb, float shininess)
{
	float diffuse[4] = { dr, dg, db, 1.f };
	float specular[4] = { sr, sg, sb, 1.f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	glutSolidSphere(radius, 100, 100);
}

void getNormalVector(GLdouble normalVector[3], const double v1[3], const double v2[3], const double v3[3]) {
	double v_rst1[3];
	double v_rst2[3];

	for (int i = 0; i < 3; i++) {
		v_rst1[i] = v1[i] - v2[i];
		v_rst2[i] = v3[i] - v2[i];
	}

	normalVector[0] = v_rst1[1] * v_rst2[2] - v_rst1[2] * v_rst2[1];
	normalVector[1] = v_rst1[2] * v_rst2[0] - v_rst1[0] * v_rst2[2];
	normalVector[2] = v_rst1[0] * v_rst2[1] - v_rst1[1] * v_rst2[0];

	double norm_rst = 0.0;
	for (int i = 0; i < 3; i++) {
		norm_rst += normalVector[i] * normalVector[i];
	}
	for (int i = 0; i < 3; i++) {
		normalVector[i] = normalVector[i] / sqrt(norm_rst);
	}
}

void displayGround() {
	float diffuse[4] = { 0.0, 0.3, 0.1, 1.f };
	float specular[4] = { 0.0, 0.3, 0.1, 1.f };
	float diffuse2[4] = { 0.6, 0.6, 0.6, 1.f };
	float specular2[4] = { 0.6, 0.6, 0.6, 1.f };
	GLdouble normal[3] = { 0.0,0.0,0.0 };
	getNormalVector(normal, vertices[0], vertices[1], vertices[2]);
	glNormal3d(normal[0], normal[1], normal[2]);
	glPushMatrix();
	glTranslated(75, 0.0, 75);
	for (int i = 0; i < 10; i++) {
		glTranslated(-15, 0.0, 0.0);
		for (int j = 0; j < 10; j++) {
			glTranslated(0.0, 0.0, -15);
			if ((i % 2 == 0 && j % 2 == 0) || (i % 2 != 0 && j % 2 != 0)) {
				
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 5.f);
				glBegin(GL_QUADS);
			}
				
			else
				glBegin(GL_LINE_LOOP);
			for (int k = 0; k < 4; k++)
				glVertex3dv(vertices[k]);
			glEnd();
		}
		glTranslated(0.0, 0.0, 150);
	}
	
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-500.f, -0.1f, -500.f);
	glScalef(1000, 1, 1000);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse2);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular2);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.f);
	glBegin(GL_QUADS);
	glVertex3dv(vertices[0]);
	glVertex3dv(vertices[1]);
	glVertex3dv(vertices[2]);
	glVertex3dv(vertices[3]);
	glEnd();
	glPopMatrix();

}

void displayAxis(float length)
{
	glBegin(GL_LINES);
	glColor3f(1.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(length, 0.f, 0.f);

	glColor3f(0.f, 1.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, length, 0.f);

	glColor3f(0.f, 0.f, 1.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, length);
	glEnd();
	
}

void calc_status() {
	float phi_yaw;
	float phi_pitch;
	float phi_roll;
	
	phi_yaw = atan2(stats_self[HEADI][0], -stats_self[HEADI][2]);

	float x_ = cos(phi_yaw) * stats_self[TOP][0] + sin(phi_yaw) * -stats_self[TOP][2];
	float y_ = stats_self[TOP][1];
	float z_ = -sin(phi_yaw) * stats_self[TOP][0] + cos(phi_yaw) * -stats_self[TOP][2];

	phi_pitch = atan2(-sin(phi_yaw) * stats_self[HEADI][0] + cos(phi_yaw) * stats_self[HEADI][2],
		stats_self[HEADI][1]);
	
	phi_roll = atan2(x_,y_*cos(phi_pitch+M_PI/2)-z_*sin(phi_pitch + M_PI / 2));
	yaw_ang =  phi_yaw * 180 /M_PI;
	pitch_ang =phi_pitch * -180 / M_PI;
	roll_ang = phi_roll * 180 / M_PI;
	printf("y %.0f\n", yaw_ang);
	printf("p %.0f\n", pitch_ang);
	printf("r %.0f\n", roll_ang);
	printf("%f", pow_self);
}

void genIndicator() {
	glColor3f(0.f, 1.f, .6f);
	for (int i = 0; i < 36; i ++) {
		if (i % 3 == 0) {
			glColor3f(.8f, .8f, .8f);
		}
		else {
			glColor3f(0.f, 1.f, .6f);
			if (i < 27 && i > 9)glColor3f(1.f, 0.f, 1.f);
		}
		if(i % 9 == 0)glColor3f(0.f, 1.f, 1.f);
		
		glBegin(GL_LINES);
		glVertex3f(3.1f, 15.f*cos(i*M_PI/18), 15.f * sin(i * M_PI / 18));
		glVertex3f(1.5f, 15.f * cos(i * M_PI / 18), 15.f * sin(i * M_PI / 18));
		glVertex3f(-3.1f, 15.f * cos(i * M_PI / 18), 15.f * sin(i * M_PI / 18));
		glVertex3f(-1.5f, 15.f * cos(i * M_PI / 18), 15.f * sin(i * M_PI / 18));
		glEnd();

	}
}


void keyboardFunction(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	case 'w':
		pitch_upd += 0.001;
		break;
	case 's':
		pitch_upd += -0.001;
		break;
	case 'a':
		roll_upd += 0.001;
		break;
	case 'd':
		roll_upd += -0.001;
		break;
	case 'q':
		yaw_upd += 0.001;
		break;
	case 'e':
		yaw_upd += -0.001;
		break;
	case 'r':
		if (pow_self < 2.0) {
			pow_self += 0.05f;
		}
		else {
			pow_self = 2.0f;
		}
		break;
	case 'f':
		if (pow_self > 0) {
			pow_self -= 0.05f;
		}
		else {
			pow_self = 0.f;
		}
		break;
	case 'h':
		stick = stick ^ 1;
		printf("%d\n", stick);
		pitch_upd = 0;
		roll_upd = 0;
		yaw_upd = 0;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void passivemotion(int x, int y) {
	if (stick == 0)return;
	if (495 < x && x < 505 && 495 < y && y < 505) {
		roll_upd = 0;
		pitch_upd = 0;
		return; }
	roll_upd = (WINDOW_WIDTH / 2 - x)/20000.f;
	pitch_upd = (WINDOW_HEIGHT / 2 - y)/20000.f;
}

void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	setCamera();
	setLight();

	displayAxis(1000.f);
	


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	displayGround();
	glPushMatrix();

	glTranslatef(stats_self[POSIT][0], stats_self[POSIT][1], stats_self[POSIT][2]);


	glPushMatrix();
	glTranslatef(stats_self[HEADI][0], stats_self[HEADI][1], stats_self[HEADI][2]);
	//displaySphere(.1f, 0.f, 0.3f, 1.f, 0.3f, 0.3f, 0.3f, 30.f);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(stats_self[TOP][0], stats_self[TOP][1], stats_self[TOP][2]);
	//displaySphere(.1f, 0.f, 1.f, 0.3f, 0.3f, 0.3f, 0.3f, 30.f);
	glPopMatrix();


	glPushMatrix();
	glRotatef(-yaw_ang, 0.f, 1.f, 0.f);
	glDisable(GL_LIGHT0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	genIndicator();
	glPopMatrix();

	glPopMatrix();

	glLoadIdentity();




	glPushMatrix();
	glTranslated(-2.1, -2., -25);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	displayTextureplate();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();
	

	glutSwapBuffers();
	
}


void stats_intg(int id0,int id1) {
	stats_self[id1][0] += stats_self[id0][0];
	stats_self[id1][1] += stats_self[id0][1];
	stats_self[id1][2] += stats_self[id0][2];
}

int count = 0;
void update(void)
{
	count++;
	if (count % 10 != 0) return;
	update_posture(roll_upd,pitch_upd,yaw_upd);
	stats_intg(THRUST, VELOC);
	stats_intg(VELOC, POSIT);
	if (stats_self[POSIT][1]<-.5f) {
		stats_self[POSIT][1] = 0;
		stats_self[VELOC][1] = 0;
	}
	calc_status();
	glutPostRedisplay();

}



void RRfunc_stats(float nx, float ny, float nz, const int id, float rot) {
	float px_in = stats_self[id][0];
	float py_in = stats_self[id][1];
	float pz_in = stats_self[id][2];

	stats_self[id][0] = (nx * nx * (1 - cos(rot)) + cos(rot)) * px_in
		+ (nx * ny * (1 - cos(rot)) - nz * sin(rot)) * py_in
		+ (nx * nz * (1 - cos(rot)) + ny * sin(rot)) * pz_in;

	stats_self[id][1] = (nx * ny * (1 - cos(rot)) + nz * sin(rot)) * px_in
		+ (ny * ny * (1 - cos(rot)) + cos(rot)) * py_in
		+ (ny * nz * (1 - cos(rot)) - nx * sin(rot)) * pz_in;

	stats_self[id][2] = (nx * nz * (1 - cos(rot)) - ny * sin(rot)) * px_in
		+ (ny * nz * (1 - cos(rot)) + nx * sin(rot)) * py_in
		+ (nz * nz * (1 - cos(rot)) + cos(rot)) * pz_in;
}


void update_posture(float rr, float pr, float yr) {
	float hx_in = stats_self[HEADI][0];
	float hy_in = stats_self[HEADI][1];
	float hz_in = stats_self[HEADI][2];

	float tx_in = stats_self[TOP][0];
	float ty_in = stats_self[TOP][1];
	float tz_in = stats_self[TOP][2];

	float hXtx = stats_self[HEADI][1] * stats_self[TOP][2] - stats_self[HEADI][2] * stats_self[TOP][1];
	float hXty = stats_self[HEADI][2] * stats_self[TOP][0] - stats_self[HEADI][0] * stats_self[TOP][2];
	float hXtz = stats_self[HEADI][0] * stats_self[TOP][1] - stats_self[HEADI][1] * stats_self[TOP][0];

	float size = sqrt(hXtx * hXtx + hXty * hXty + hXtz * hXtz);
	hXtx /= size;
	hXty /= size;
	hXtz /= size;

	RRfunc_stats(hx_in, hy_in, hz_in, HEADI, rr);
	RRfunc_stats(hx_in, hy_in, hz_in, TOP, rr);

	RRfunc_stats(tx_in, ty_in, tz_in, HEADI, yr);
	RRfunc_stats(tx_in, ty_in, tz_in, TOP, yr);


	RRfunc_stats(hXtx, hXty, hXtz, HEADI, pr);
	RRfunc_stats(hXtx, hXty, hXtz, TOP, pr);
	stats_self[THRUST][0] = stats_self[TOP][0]* GRAVITY * pow_self;
	stats_self[THRUST][1] = stats_self[TOP][1]* GRAVITY * pow_self - GRAVITY;
	stats_self[THRUST][2] = stats_self[TOP][2]* GRAVITY * pow_self;
	printf("POSIT x:%.4f y:%.4f z:%.4f\n", stats_self[POSIT][0], stats_self[POSIT][1], stats_self[POSIT][2]);
}

void initialize_stats() {
	for (int i = 0; i < 5; i++) {
		stats_self[i][0] = 0.f;
		stats_self[i][1] = 0.f;
		stats_self[i][2] = 0.f;
	}
	stats_self[HEADI][2] = 1.f;
	stats_self[POSIT][2] = 1.f;
	stats_self[TOP][1] = 1.f;
	pow_self = 1.f;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("hovering simulator");
	glClearColor(0.4f, 0.4f, 0.9f, 0.f);
	
	glDepthFunc(GL_LESS);
	textureID = loadTexture("lighting_mech_sq_512.bmp");

	initialize_stats();
	printf("init\n\n\n");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboardFunction);
	glutPassiveMotionFunc(passivemotion);
	glutIdleFunc(update);
	glutMainLoop();

	return 0;

}