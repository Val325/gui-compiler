#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#define NK_IMPLEMENTATION
#include <nuklear.h>
#include "nuklear_glfw_gl3.h"
#include "tinyfiledialogs.c"

//#define WINDOW_WIDTH 600
//#define WINDOW_HEIGHT 400

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 400;
int main() {
  GLFWwindow* window;
  struct nk_context *ctx;
  int width = 0, height = 0;
  struct nk_colorf bg;
  //path to includes
  std::string path_includes = "";
  //path to libs
  std::string path_libs = "";
  

   /* Initialize the library */
  if (!glfwInit())
    return -1;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "My programm", NULL, NULL);
  glfwMakeContextCurrent(window);
  glfwGetWindowSize(window, &width, &height);
  if (!window){
    glfwTerminate();
    return -1;
  }
  /* OpenGL */ 
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
  glewExperimental = 1;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to setup GLEW\n");
  exit(1);
  }
  ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
  //add fonts
  {struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "DroidSans.ttf", 14, 0);
    nk_glfw3_font_stash_end();
    nk_style_set_font(ctx, &droid->handle);
  }
  /* Make the window's context current */
  glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
  bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
  while (!glfwWindowShouldClose(window)){
    /* Render here */
    /* Input */
    glfwPollEvents();
    nk_glfw3_new_frame();
      //here gui 
    enum {GCC, GPLUS};
    static int op = GCC;
    static float value = 0.6f;
    static int i =  20;
    std::string compilerStr;
    if (nk_begin(ctx, "miniguicompiler", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
      NK_WINDOW_BORDER)) {
      /* fixed widget pixel width */
      //nk_layout_row_static(ctx, 20, 80, 2);
      //if (nk_button_label(ctx, "button")) {
        /* event handling */
      //}
      // if (nk_button_label(ctx, "delete being")) {
        /* event handling */
      //}
      nk_label(ctx, "  Which compiler you want using?", NK_TEXT_RIGHT);


      /* fixed widget window ratio width */
      //nk_layout_row_dynamic(ctx, 30, 2);
      nk_layout_row_static(ctx, 30, 100, 1);
      if (nk_option_label(ctx, "gcc", op == GCC)){
        op = GCC;
        compilerStr = "gcc";
      } 
      if (nk_option_label(ctx, "g++", op == GPLUS)){
         op = GPLUS;
         compilerStr = "g++";
      }
      nk_label(ctx, "  Yours folders for compilation:", NK_TEXT_RIGHT);

      nk_layout_row_static(ctx, 30, 150, 2);
      if (nk_button_label(ctx, "Select includes folders")){
        path_includes = tinyfd_selectFolderDialog("select include folders", "pwd");
        std::cout << "includes:" << path_includes << "\n";
      }
      if (nk_button_label(ctx, "Select libs folders")){
        path_libs = tinyfd_selectFolderDialog("select libs folders", "pwd");
        std::cout << "libs:" << path_libs << "\n";
      }
      nk_layout_row_static(ctx, 30, 0, 1);

      if (path_includes.c_str() != "") {
        std::string includes_text = "includes: ";
        std::string full_label_path_lncludes = includes_text + path_includes;

        nk_label(ctx, full_label_path_lncludes.c_str(), NK_TEXT_RIGHT);
      }
      if (path_libs.c_str() != "-I") {
        std::string libs_text = "libs: ";
        std::string full_label_path_lib = libs_text + path_libs;
        nk_label(ctx, full_label_path_lib.c_str(), NK_TEXT_RIGHT);

      }
      nk_layout_row_static(ctx, 30, 200, 1);
      
      std::string includes_text = "-I";
      std::string libs_text = "libs: ";
      static char buf[256] = {0};
      static int text_len = 0;

      // in window
      //nk_edit_string_zero_terminated (ctx, NK_EDIT_FIELD, buf, sizeof(buf) - 1, nk_filter_default);
      nk_layout_row_static(ctx, 30, 0, 1);
      nk_label(ctx, "selected file", NK_TEXT_RIGHT);
      
    
      static char buf_sel[256];
      buf_sel[0] = '/';
      static int text_len_sel = 0;

      nk_layout_row_static(ctx, 30, 200, 1);
      std::string path_sel_file = "";

      nk_edit_string(ctx, NK_EDIT_SIMPLE, buf_sel,&text_len_sel, 64, nk_filter_default);
      if (nk_button_label(ctx, "selected file")){
        path_sel_file = tinyfd_openFileDialog("","",0, NULL,"main file",20);
        int size_sel_file_char = sizeof(path_sel_file) - 1;
        text_len_sel = size_sel_file_char;
        //strcpy(buf_sel, path_sel_file.c_str());
        //std::copy(std::begin(buf_sel), std::end(buf_sel), std::begin(path_sel_file));
        std::cout << "path select file" << path_sel_file << "\n";
      }
      
      strcpy(buf_sel, path_sel_file.c_str());

	      //printf ("%s\n", buf);
      
      nk_layout_row_static(ctx, 30, 0, 1);
      nk_label(ctx, "output file file", NK_TEXT_RIGHT);
      std::string path_output_file = "";
      nk_layout_row_static(ctx, 30, 200, 1);
      nk_edit_string(ctx, NK_EDIT_SIMPLE, buf,&text_len , 64, nk_filter_default);
      if (nk_button_label(ctx, "output file!")){
        printf ("%s\n", buf);

      }

      nk_layout_row_static(ctx, 30, 0, 1);
      nk_label(ctx, "compiler generated command:", NK_TEXT_RIGHT);
      
      nk_layout_row_static(ctx, 30, 200, 1);
      nk_edit_string(ctx, NK_EDIT_SIMPLE, buf,&text_len , 64, nk_filter_default);
      if (nk_button_label (ctx, "compile!"))
	      printf ("%s\n", buf);
      /* custom widget pixel width */
      //nk_layout_row_begin(ctx, NK_STATIC, 30, 2);
      //{
      //  nk_layout_row_push(ctx, 50);
      //  nk_label(ctx, "Volume:", NK_TEXT_LEFT);
      //  nk_layout_row_push(ctx, 110);
      //  nk_slider_float(ctx, 0, &value, 1.0f, 0.1f);
      //}
      //nk_layout_row_end(ctx);
    }
    nk_end(ctx);

    glClear(GL_COLOR_BUFFER_BIT);
    
  
    /* Draw */
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(bg.r, bg.g, bg.b, bg.a);
        /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state after
         * rendering the UI. */
     nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }
    nk_glfw3_shutdown();
    glfwTerminate();
    return 0;
}


