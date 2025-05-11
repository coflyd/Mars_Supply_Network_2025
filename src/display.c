
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glut.h> // Pour glutSolidSphere
#include <GL/glu.h>  // Pour gluPerspective

#include "structures.h"

void drawSphere(float radius, int lats, int longs) {
	glColor3f(0.8f, 0.4f, 0.1f); // Couleur rouge-orangé pour Mars
	for (int i = 0; i < lats; i++) {
		float lat0 = M_PI * (-0.5 + (float) (i - 1) / lats);
		float z0 = sin(lat0);
		float zr0 = cos(lat0);

		float lat1 = M_PI * (-0.5 + (float) i / lats);
		float z1 = sin(lat1);
		float zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= longs; j++) {
			float lng = 2 * M_PI * (float) (j - 1) / longs;
			float x = cos(lng);
			float y = sin(lng);

			glVertex3f(x * zr0 * radius, y * zr0 * radius, z0 * radius);
			glVertex3f(x * zr1 * radius, y * zr1 * radius, z1 * radius);
		}
		glEnd();
	}
}
GLuint renderTextToTexture(const char *text, TTF_Font *font, SDL_Color color,
		int *w, int *h) {
	if (!text || !font)
		return 0;

	SDL_Surface *raw = TTF_RenderUTF8_Blended(font, text, color);
	if (!raw) {
		fprintf(stderr, "Erreur rendu texte : %s\n", TTF_GetError());
		return 0;
	}

	SDL_Surface *surface = SDL_ConvertSurfaceFormat(raw,
			SDL_PIXELFORMAT_ABGR8888, 0);
	SDL_FreeSurface(raw);
	if (!surface) {
		fprintf(stderr, "Erreur conversion surface : %s\n", SDL_GetError());
		return 0;
	}

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
	GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	*w = surface->w;
	*h = surface->h;

	// DEBUG : export BMP pour voir le rendu du texte
	// SDL_SaveBMP(surface, "debug_text.bmp");

	SDL_FreeSurface(surface);
	return texture;
}

void drawCube(float size) {
	float hs = size / 2.0f;

	glBegin(GL_QUADS);
	// Face avant
	glVertex3f(-hs, -hs, hs);
	glVertex3f(hs, -hs, hs);
	glVertex3f(hs, hs, hs);
	glVertex3f(-hs, hs, hs);

	// Face arrière
	glVertex3f(-hs, -hs, -hs);
	glVertex3f(-hs, hs, -hs);
	glVertex3f(hs, hs, -hs);
	glVertex3f(hs, -hs, -hs);

	// Face gauche
	glVertex3f(-hs, -hs, -hs);
	glVertex3f(-hs, -hs, hs);
	glVertex3f(-hs, hs, hs);
	glVertex3f(-hs, hs, -hs);

	// Face droite
	glVertex3f(hs, -hs, -hs);
	glVertex3f(hs, hs, -hs);
	glVertex3f(hs, hs, hs);
	glVertex3f(hs, -hs, hs);

	// Face haut
	glVertex3f(-hs, hs, -hs);
	glVertex3f(-hs, hs, hs);
	glVertex3f(hs, hs, hs);
	glVertex3f(hs, hs, -hs);

	// Face bas
	glVertex3f(-hs, -hs, -hs);
	glVertex3f(hs, -hs, -hs);
	glVertex3f(hs, -hs, hs);
	glVertex3f(-hs, -hs, hs);
	glEnd();
}



void drawNodeLabel(Node *node) {
	static TTF_Font *font = NULL;
	if (!font) {
		font = TTF_OpenFont("data/arial.ttf", 24);
		if (!font) {
			fprintf(stderr, "Erreur chargement police : %s\n", TTF_GetError());
			return;
		}
	}

	int texW, texH;
	SDL_Color white = { 255, 255, 255, 255 };

	const char *text = node->name ? node->name : "???";
	GLuint texId = renderTextToTexture(text, font, white, &texW, &texH);
	if (texId == 0)
		return;

	// Position radiale étendue
	float offset = 40.0f;
	float tx = node->x * (1.0f + offset / SPHERE_RADIUS);
	float ty = node->y * (1.0f + offset / SPHERE_RADIUS);
	float tz = node->z * (1.0f + offset / SPHERE_RADIUS);

	glPushMatrix();
	glTranslatef(tx, ty, tz);

	// Billboard
	/*
	 * GLfloat mv[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);
	float billboard[16] = { mv[0], mv[4], mv[8], 0, mv[1], mv[5], mv[9], 0,
			mv[2], mv[6], mv[10], 0, 0, 0, 0, 1 };
	billboard[1] = -billboard[1];
	billboard[5] = -billboard[5];
	billboard[9] = -billboard[9];
	glMultMatrixf(billboard);
*/

	// Cylindrical billboard : rotation seulement autour de Y (caméra)
	// GLfloat mv[16];
	// glGetFloatv(GL_MODELVIEW_MATRIX, mv);

	// Extraire l’angle horizontal caméra (axe Y)
	// float angle = atan2(mv[2], mv[0]) * (180.0f / M_PI); // angle en degrés

	// Appliquer une rotation inverse autour de Y pour orienter le texte vers la caméra
	// glRotatef(-angle, 0.0f, 1.0f, 0.0f);

	 GLfloat mv[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mv);

	// Annuler toute rotation (billboard complet)
	float billboard[16] = {
	    mv[0], mv[4], mv[8],  0.0f,
	    mv[1], mv[5], mv[9],  0.0f,
	    mv[2], mv[6], mv[10], 0.0f,
	    0.0f,  0.0f,  0.0f,   1.0f
	};

	// Inverser l’orientation verticale du texte (à tester)
	billboard[1] *= -1.0f;
	billboard[5] *= -1.0f;
	billboard[9] *= -1.0f;

	glMultMatrixf(billboard);

	// Texte
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texId);
	glColor3f(1.0f, 1.0f, 1.0f);

	float scale = 0.005f;
	glScalef(scale * texW, scale * texH, 1.0f);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 1);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glTexCoord2f(1, 1);
	glVertex3f(0.5f, 0.5f, 0.0f);
	glTexCoord2f(1, 0);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glTexCoord2f(0, 0);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPopMatrix();

	glDeleteTextures(1, &texId);
}

void drawNode(Node *node) {
	// Couleur par type d'infrastructure
	switch (node->infrastructure) {
	case DOME:
		glColor3f(1.0f, 0.0f, 0.0f);
		break;
	case RESEARCH_STATION:
		glColor3f(0.0f, 0.0f, 1.0f);
		break;
	case SUPPLY_DEPOT:
		glColor3f(0.0f, 1.0f, 0.0f);
		break;
	default:
		glColor3f(1.0f, 1.0f, 1.0f);
		break;
	}

	// Dessiner le cube
	glPushMatrix();
	glTranslatef(node->x, node->y, node->z);
	drawCube(node->radius);
	glPopMatrix();

	// Trait radial vers le label
	float offset = 40.0f;

	float x1 = node->x;
	float y1 = node->y;
	float z1 = node->z;

	float x2 = node->x * (1.0f + offset / SPHERE_RADIUS);
	float y2 = node->y * (1.0f + offset / SPHERE_RADIUS);
	float z2 = node->z * (1.0f + offset / SPHERE_RADIUS);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glLineWidth(1.0f);
	glColor3f(0.6f, 0.6f, 0.6f);
	glBegin(GL_LINES);
	glVertex3f(x1, y1, z1);
	glVertex3f(x2, y2, z2);
	glEnd();

	drawNodeLabel(node);
}

void drawEdge(Edge *edge, int segments) {
	// Définir la couleur en fonction de l'état de la route
	switch (edge->condition) {
	case FUNCTIONAL:
		glColor3f(0.0f, 1.0f, 1.0f); // Vert pour les routes fonctionnelles
		break;
	case DAMAGE:
		glColor3f(1.0f, 1.0f, 0.0f); // Jaune pour les routes endommagées
		break;
	case DESTROYED:
		glColor3f(1.0f, 0.0f, 0.0f); // Rouge pour les routes détruites
		break;
	default:
		glColor3f(1.0f, 1.0f, 1.0f); // Blanc par défaut (au cas où)
		break;
	}

	glBegin(GL_LINE_STRIP);

	Node start = *edge->start;
	Node end = *edge->end;

	for (int i = 0; i <= segments; i++) {
		float t = (float) i / segments;
		float x = start.x * (1 - t) + end.x * t;
		float y = start.y * (1 - t) + end.y * t;
		float z = start.z * (1 - t) + end.z * t;

		// Normaliser le point pour qu'il soit sur la surface de la sphère
		float length = sqrt(x * x + y * y + z * z);
		x /= length;
		y /= length;
		z /= length;

		glVertex3f(x, y, z);
	}

	glEnd();
}

void handleEvents(SDL_Event *event, Display *display, UIState *ui) {
    while (SDL_PollEvent(event)) {
        if (event->type == SDL_QUIT) {
            exit(0);
        } else if (event->type == SDL_MOUSEWHEEL) {
            if (event->wheel.y > 0)
                display->zoom *= 0.9f;
            else if (event->wheel.y < 0)
                display->zoom *= 1.1f;
        } else if (event->type == SDL_MOUSEBUTTONDOWN) {
            if (event->button.button == SDL_BUTTON_LEFT) {
                int mx = event->button.x;
                int my = event->button.y;

                if (mx < 200) { // zone UI à gauche
                    // flèche haut départ
                    if (my >= 30 && my <= 45)
                        ui->selectedStart = (ui->selectedStart + 1) % 100;

                    // flèche bas départ
                    if (my >= 45 && my <= 60)
                        ui->selectedStart = (ui->selectedStart - 1 + 100) % 100;

                    // flèche haut arrivée
                    if (my >= 90 && my <= 105)
                        ui->selectedEnd = (ui->selectedEnd + 1) % 100;

                    // flèche bas arrivée
                    if (my >= 105 && my <= 120)
                        ui->selectedEnd = (ui->selectedEnd - 1 + 100) % 100;

                    // bouton "Afficher chemin"
                    if (my >= 140 && my <= 170)
                        ui->showPath = 1;

                    return;  // éviter rotation
                }

                display->mouseDown = 1;
                display->lastX = mx;
                display->lastY = my;
            }
        } else if (event->type == SDL_MOUSEBUTTONUP) {
            if (event->button.button == SDL_BUTTON_LEFT) {
                display->mouseDown = 0;
            }
        } else if (event->type == SDL_MOUSEMOTION && display->mouseDown) {
            int x, y;
            SDL_GetMouseState(&x, &y);
            display->rotX += (y - display->lastY) * 0.5f;
            display->rotY += (x - display->lastX) * 0.5f;
            display->lastX = x;
            display->lastY = y;
        }
    }
}



/*
void handleEvents(SDL_Event *event, Display *display, UIState *ui) {
	while (SDL_PollEvent(event)) {
		if (event->type == SDL_QUIT) {
			exit(0); // Quitter l'application
		} else if (event->type == SDL_MOUSEWHEEL) {
			if (event->wheel.y > 0) {
				display->zoom *= 0.9f;
			} else if (event->wheel.y < 0) {
				display->zoom *= 1.1f;
			}
		} else if (event->type == SDL_MOUSEBUTTONDOWN) {
			if (event->button.button == SDL_BUTTON_LEFT) {

				int mx = event->button.x;
				int my = event->button.y;

				if (mx < 200) { // zone interface UI
					if (my >= 30 && my <= 50) {
						ui->selectedStart = (ui->selectedStart + 1) % 100;
					}
					if (my >= 90 && my <= 110) {
						ui->selectedEnd = (ui->selectedEnd + 1) % 100;
					}
					if (my >= 140 && my <= 170) {
						ui->showPath = 1;
					}
					return; // ne pas traiter comme rotation
				}




				display->mouseDown = 1;
				display->lastX = event->button.x;
				display->lastY = event->button.y;
			}
		} else if (event->type == SDL_MOUSEBUTTONUP) {
			if (event->button.button == SDL_BUTTON_LEFT) {
				display->mouseDown = 0;
			}
		} else if (event->type == SDL_MOUSEMOTION && display->mouseDown) {
			int x, y;
			SDL_GetMouseState(&x, &y);
			display->rotX += (y - display->lastY) * 0.5f;
			display->rotY += (x - display->lastX) * 0.5f;
			display->lastX = x;
			display->lastY = y;
		}
	}
}
*/
void drawHighlightedEdge(Edge *edge, int segments) {
    glLineWidth(4.0f);  // plus épais
    glColor3f(1.0f, 1.0f, 1.0f); // Blanc éclatant pour le chemin

    glBegin(GL_LINE_STRIP);
    Node start = *edge->start;
    Node end = *edge->end;

    for (int i = 0; i <= segments; i++) {
        float t = (float) i / segments;
        float x = start.x * (1 - t) + end.x * t;
        float y = start.y * (1 - t) + end.y * t;
        float z = start.z * (1 - t) + end.z * t;
        float len = sqrt(x*x + y*y + z*z);
        glVertex3f(x / len, y / len, z / len);
    }

    glEnd();
    glLineWidth(1.0f); // reset épaisseur
}


void drawText2D(TTF_Font *font, const char *text, float x, float y, SDL_Color color) {
    if (!text || !font) return;

    SDL_Surface *textSurface = TTF_RenderUTF8_Blended(font, text, color);
    if (!textSurface) return;

    SDL_Surface *formatted = SDL_ConvertSurfaceFormat(textSurface, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_FreeSurface(textSurface);
    if (!formatted) return;

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formatted->w, formatted->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, formatted->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Passe en mode 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    float w = formatted->w;
    float h = formatted->h;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(x, y);
    glTexCoord2f(1, 0); glVertex2f(x + w, y);
    glTexCoord2f(1, 1); glVertex2f(x + w, y + h);
    glTexCoord2f(0, 1); glVertex2f(x, y + h);
    glEnd();

    glPopMatrix(); // Modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDeleteTextures(1, &textureID);
    SDL_FreeSurface(formatted);
}

void drawUIOverlay(TTF_Font *font, Node *nodes, int totalNodes, UIState *ui) {



    // Début UIOverlay
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Dessin fond + bouton (OpenGL)
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(200, 0);
    glVertex2f(200, WINDOW_HEIGHT);
    glVertex2f(0, WINDOW_HEIGHT);
    glEnd();

    // Rectangle autour du bouton
    glColor3f(0.3f, 0.3f, 0.8f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(8, 138);
    glVertex2f(192, 138);
    glVertex2f(192, 158);
    glVertex2f(8, 158);
    glEnd();

    // --- Affichage textes dans le même contexte 2D ---
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color blue = {100, 100, 255, 255};
    char buffer[64];

    drawText2D(font, "[LOOK AT THE TERMINAL]", 10, 15, blue);

    drawText2D(font, "Départ :", 10, 50, white);
    snprintf(buffer, sizeof(buffer), "[%s]  ➤", nodes[ui->selectedStart].name);
    drawText2D(font, buffer, 10, 70, white);

    drawText2D(font, "Arrivée :", 10, 90, white);
    snprintf(buffer, sizeof(buffer), "[%s]  ➤", nodes[ui->selectedEnd].name);
    drawText2D(font, buffer, 10, 110, white);

    drawText2D(font, "[Afficher chemin]", 10, 140, blue);

    // Nettoyage matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}
