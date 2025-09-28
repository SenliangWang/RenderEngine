#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Demo: fixed pipeline (vertex-colour tracking -> blue) vs shader (uniform teal)

static const GLfloat kVertices[9] = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};

static const GLfloat kNormals[9] = {
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f
};

// All vertices blue (to mimic your ColorList::Blue)
static const GLfloat kColors[12] = {
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f
};

// Teal material (matches CColor::Teal)
static const GLfloat kTeal[4] = { 0.0f, 0.5f, 0.5f, 1.0f };

// Window size
static int gWinW = 960;
static int gWinH = 600;

// Fog parameters (no toggles; constants)
static int gFogMode = 0;        // 0:none, 1:linear, 2:exp, 3:exp2
static float gFogDensity = 0.25f;
static float gFogNear = 1.0f;
static float gFogFar = 5.0f;

// Shader program + locations
static GLuint gProgram = 0;
static GLint gAttrPos = -1;
static GLint gUniColor = -1;
static GLint gUniMVP = -1;
static GLint gUniMV = -1;
static GLint gUniFogMode = -1;
static GLint gUniFogDensity = -1;
static GLint gUniFogColor = -1;
static GLint gUniFogNear = -1;
static GLint gUniFogFar = -1;

// Simple 4x4 column-major matrix multiplication: out = a * b
static void mulMat4(const GLfloat *a, const GLfloat *b, GLfloat *out) {
	for (int col = 0; col < 4; ++col) {
		for (int row = 0; row < 4; ++row) {
			out[col * 4 + row] =
				a[0 * 4 + row] * b[col * 4 + 0] +
				a[1 * 4 + row] * b[col * 4 + 1] +
				a[2 * 4 + row] * b[col * 4 + 2] +
				a[3 * 4 + row] * b[col * 4 + 3];
		}
	}
}

static GLuint compileShader(GLenum type, const char *src) {
	GLuint sh = glCreateShader(type);
	glShaderSource(sh, 1, &src, nullptr);
	glCompileShader(sh);
	GLint ok = GL_FALSE;
	glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		GLchar log[4096];
		GLint len = 0;
		glGetShaderInfoLog(sh, sizeof(log), &len, log);
		fprintf(stderr, "Shader compile error (%s):\n%.*s\n", type == GL_VERTEX_SHADER ? "VS" : "FS", len, log);
		glDeleteShader(sh);
		return 0;
	}
	return sh;
}

static GLuint linkProgram(GLuint vs, GLuint fs) {
	GLuint prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	glLinkProgram(prog);
	GLint ok = GL_FALSE;
	glGetProgramiv(prog, GL_LINK_STATUS, &ok);
	if (!ok) {
		GLchar log[4096];
		GLint len = 0;
		glGetProgramInfoLog(prog, sizeof(log), &len, log);
		fprintf(stderr, "Program link error:\n%.*s\n", len, log);
		glDeleteProgram(prog);
		return 0;
	}
	return prog;
}

static const char *kVertexSrc =
	"#version 120\n"
	"attribute vec3 attrVertex;\n"
	"uniform vec4 vColor;\n"
	"uniform mat4 mvpMatrix;\n"
	"uniform mat4 mvMatrix;\n"
	"varying vec4 vFragColor;\n"
	"varying vec4 vViewPos;\n"
	"void main() {\n"
	"  vFragColor = vColor;\n"
	"  gl_Position = mvpMatrix * vec4(attrVertex, 1.0);\n"
	"  vViewPos = mvMatrix * vec4(attrVertex, 1.0);\n"
	"}\n";

static const char *kFragmentSrc =
	"#version 120\n"
	"varying vec4 vFragColor;\n"
	"varying vec4 vViewPos;\n"
	"uniform int fogMode;\n"
	"uniform float fogDensity;\n"
	"uniform vec4 fogColor;\n"
	"uniform float fogNear;\n"
	"uniform float fogFar;\n"
	"void main() {\n"
	"  vec4 c = vFragColor;\n"
	"  float fogDepth = -vViewPos.z;\n"
	"  if (fogMode == 1) {\n"
	"    float fogFactor = clamp((fogDepth - fogNear) / max(0.0001, (fogFar - fogNear)), 0.0, 1.0);\n"
	"    c.rgb = mix(c.rgb, fogColor.rgb, fogFactor);\n"
	"  } else if (fogMode == 2) {\n"
	"    float fogFactor = 1.0 - exp(-fogDensity * fogDepth);\n"
	"    c.rgb = mix(c.rgb, fogColor.rgb, fogFactor);\n"
	"  } else if (fogMode == 3) {\n"
	"    float fogFactor = 1.0 - exp(-fogDensity*fogDensity*fogDepth*fogDepth);\n"
	"    c.rgb = mix(c.rgb, fogColor.rgb, fogFactor);\n"
	"  }\n"
	"  gl_FragColor = c;\n"
	"}\n";

static void initProgram() {
	GLuint vs = compileShader(GL_VERTEX_SHADER, kVertexSrc);
	GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFragmentSrc);
	if (!vs || !fs) {
		return;
	}
	gProgram = linkProgram(vs, fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	if (!gProgram) return;

	// Query locations
	gAttrPos = glGetAttribLocation(gProgram, "attrVertex");
	gUniColor = glGetUniformLocation(gProgram, "vColor");
	gUniMVP = glGetUniformLocation(gProgram, "mvpMatrix");
	gUniMV = glGetUniformLocation(gProgram, "mvMatrix");
	gUniFogMode = glGetUniformLocation(gProgram, "fogMode");
	gUniFogDensity = glGetUniformLocation(gProgram, "fogDensity");
	gUniFogColor = glGetUniformLocation(gProgram, "fogColor");
	gUniFogNear = glGetUniformLocation(gProgram, "fogNear");
	gUniFogFar = glGetUniformLocation(gProgram, "fogFar");
}

// Fixed-pipeline setup removed; we always render with shader below

static void setShaderPipelineState() {
    glUseProgram(gProgram);

    // Match your state order/intent
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[4] = { 0.0f, 0.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); // Two-sided lighting
    glDisable(GL_CULL_FACE); // FM_FRONT_AND_BACK
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, kTeal);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); // Vertex colour tracking

    // Provide attribute from client memory (shader uses only positions)
    glEnableVertexAttribArray((GLuint)gAttrPos);
    glVertexAttribPointer((GLuint)gAttrPos, 3, GL_FLOAT, GL_FALSE, 0, kVertices);

    // Uniforms that match your shader
    glUniform4fv(gUniColor, 1, kTeal);
    const GLfloat fogClr[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
    glUniform4fv(gUniFogColor, 1, fogClr);
    glUniform1i(gUniFogMode, gFogMode);
    glUniform1f(gUniFogDensity, gFogDensity);
    glUniform1f(gUniFogNear, gFogNear);
    glUniform1f(gUniFogFar, gFogFar);
}

static void setupMatricesAndMaybeUpload() {
	// Setup classic fixed-pipeline matrices so we can also fetch them for shader
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Use a simple perspective
	const float fovy = 60.0f;
	const float aspect = (gWinH == 0) ? 1.0f : (float)gWinW / (float)gWinH;
	const float zNear = 0.1f;
	const float zFar = 10.0f;
	// Manual gluPerspective
	{
		float f = 1.0f / tanf(fovy * 0.5f * 3.1415926535f / 180.0f);
		GLfloat P[16] = {
			f / aspect, 0, 0, 0,
			0, f, 0, 0,
			0, 0, (zFar + zNear) / (zNear - zFar), -1,
			0, 0, (2 * zFar * zNear) / (zNear - zFar), 0
		};
		glLoadMatrixf(P);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Camera at (0,0,2) looking at origin
	{
		GLfloat MV[16] = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,-2,1
		};
		glLoadMatrixf(MV);
	}

    // Upload MV and MVP to shader every frame
    if (gProgram) {
        GLfloat mv[16];
        GLfloat proj[16];
        GLfloat mvp[16];
        glGetFloatv(GL_MODELVIEW_MATRIX, mv);
        glGetFloatv(GL_PROJECTION_MATRIX, proj);
        mulMat4(proj, mv, mvp);
        glUniformMatrix4fv(gUniMV, 1, GL_FALSE, mv);
        glUniformMatrix4fv(gUniMVP, 1, GL_FALSE, mvp);
    }
}

static void display() {
	glClearColor(0.08f, 0.08f, 0.09f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, gWinW, gWinH);

    setShaderPipelineState();

    setupMatricesAndMaybeUpload();
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray((GLuint)gAttrPos);

	glutSwapBuffers();
}

static void reshape(int w, int h) {
	gWinW = w; gWinH = h;
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}

static void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: // ESC
            std::exit(0);
            break;
    }
}

static void printHelp() {
    printf("FreeGLUT demo: shader always ON (uniform teal)\n");
    printf(" - ESC: quit\n");
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(gWinW, gWinH);
    glutCreateWindow("Shader Teal Demo (FreeGLUT)");

	// Init GLEW after creating context
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		fprintf(stderr, "GLEW error: %s\n", glewGetErrorString(err));
		return 1;
	}

    printHelp();

    // Create shader program (always used)
	initProgram();
    if (!gProgram) {
        fprintf(stderr, "Error: shader program failed to compile/link.\n");
        return 1;
    }

	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}

