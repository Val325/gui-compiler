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
#include <vector>

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024



GLFWwindow* window;
struct nk_context *ctx;
int width = 0, height = 0;
struct nk_colorf bg;
enum {GCC, GPLUS};
static int op = GCC;
static float value = 0.6f;
static int i =  20;
std::string compilerStr;
//
// includes
//

//path to includes
char const * path_includes;
std::vector<std::string> arrays_all_includes_path;



// libs
char const * path_libs;
std::vector<std::string> arrays_all_libs_path;

//output
std::string path_out_file = "";
std::string path_out_file2 = "";


//select
std::string path_sel_file2 = "";

//output command
std::string path_output_file = "";

//symbols
std::string includes_text = "-I";
std::string libs_text = "-l";

//all libraries
std::string all_libraries = "";
std::vector<std::string> arrays_all_libs;

//main buf command
static char buf[256];
static int text_len = 0;

//select
static char buf_sel[256];
//buf_sel[0] = ' ';
static int text_len_sel = 0;


//output
static char buf_output[256];
static int text_len_output = 0;

//compile
static char buf_compile[256];
static int text_len_compile = 0;

int WINDOW_WIDTH = 600;
int WINDOW_HEIGHT = 400;
int INPUT_WIDTH = WINDOW_WIDTH - 40;


void select_includes(){
  if (nk_button_label(ctx, "Select includes folders")){
     
    path_includes = tinyfd_selectFolderDialog("select include folders", NULL);
    if (!path_includes) {
      return;
    }
    std::cout << "includes:" << path_includes << "\n";
    
    //std::cout << "includes:" << path_includes << "\n";
  }

}

void select_libs(){
   if (nk_button_label(ctx, "Select libs folders (windows)")){
    path_libs = tinyfd_selectFolderDialog("select libs folders", "");
     if (!path_libs) {
      return;
    }
    std::cout << "libs:" << path_libs << "\n";
  }
}

void add_libraries(){
  //
}
void delete_libraries(){
  //
}

int main(){
  
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
    if (nk_begin(ctx, "miniguicompiler", nk_rect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT),
      NK_WINDOW_BORDER)) {
     
      nk_label(ctx, "  Which compiler you want using?", NK_TEXT_RIGHT);


      /* fixed widget window ratio width */
      //nk_layout_row_dynamic(ctx, 30, 2);
      nk_layout_row_static(ctx, 30, 100, 1);
      if (nk_option_label(ctx, "gcc", op == GCC)){
        op = GCC;
        compilerStr = "gcc ";
      } 
      if (nk_option_label(ctx, "g++", op == GPLUS)){
         op = GPLUS;
         compilerStr = "g++ ";
      }
      nk_label(ctx, "  Yours folders for compilation:", NK_TEXT_RIGHT);
      
      nk_layout_row_static(ctx, 30, 150, 2);
      //
      //buttons
      //
      select_includes();
      select_libs();
      
     
      nk_layout_row_static(ctx, 30, 0, 1);
     
      
      if (path_includes != NULL) {
        std::string includes_text = "includes: ";
        std::string includ_cplus(includes_text);
        std::string full_label_path_lncludes = includ_cplus + path_includes;

        nk_label(ctx, full_label_path_lncludes.c_str(), NK_TEXT_RIGHT);
      }
      if (path_libs != NULL) {
        std::string libs_text = "libs: ";
        std::string full_label_path_lib = libs_text + path_libs;
        nk_label(ctx, full_label_path_lib.c_str(), NK_TEXT_RIGHT);

      }
    
      // in window


      //************************
      // SELECT
      //************************
      if (nk_tree_push(ctx, NK_TREE_TAB, "Selected files", NK_MINIMIZED)) {
        nk_layout_row_static(ctx, 30, 0, 1);
        nk_label(ctx, "selected main file", NK_TEXT_RIGHT);
      
      

        buf_sel[0] = ' ';
    
      
        nk_layout_row_static(ctx, 30, WINDOW_WIDTH - 40, 1);
        std::string path_sel_file = "";
    
        nk_edit_string(ctx, NK_EDIT_SIMPLE, buf_sel,&text_len_sel, 256, nk_filter_default);
        if (nk_button_label(ctx, "selected file")){
          path_sel_file = tinyfd_openFileDialog("","",0, NULL,"main file",20);

          int size_sel_file_char = path_sel_file.length();

          text_len_sel = size_sel_file_char;

          std::cout << "path select file" << path_sel_file << "\n";
          path_sel_file2 = path_sel_file;
        }

        strcpy(buf_sel, path_sel_file.c_str());
      
      //************************
      // Output
      //************************

        nk_layout_row_static(ctx, 30, 0, 1);
        nk_label(ctx, "output file name", NK_TEXT_RIGHT);
        //std::string path_output_file = "";
        nk_layout_row_static(ctx, 30, INPUT_WIDTH, 1);
        nk_edit_string(ctx, NK_EDIT_SIMPLE,buf_output , &text_len_output, 64, nk_filter_default);
        if (nk_button_label(ctx, "output file!")){
          std::cout << "output file " << buf_output << "\n";
          std::cout << "generation compiler command.." << "\n";
          std::string output_sym = " -o ";
          std::string output_data(buf_output);
          int size_sel_compile_char = path_output_file.length();
          text_len_compile = size_sel_compile_char;

          path_output_file = compilerStr + path_sel_file2 + output_sym + output_data;
          std::cout << "command " << path_output_file << "\n";
        }
        strcpy(buf_compile, path_output_file.c_str());
      //************************
      // Compile
      //************************
     
        nk_layout_row_static(ctx, 30, 0, 1);
        nk_label(ctx, "compiler generated command:", NK_TEXT_RIGHT);

        nk_layout_row_static(ctx, 30, INPUT_WIDTH, 1);
        nk_edit_string(ctx, NK_EDIT_SIMPLE, buf_compile,&text_len_compile , 64, nk_filter_default);
        strcpy(buf_compile, path_output_file.c_str());
        nk_tree_pop(ctx);
    }
  }
    if (nk_tree_push(ctx, NK_TREE_TAB, "Selected libs", NK_MINIMIZED)) {
        //bool isClicked = false;
        static int glfw_lib = 1;

        nk_checkbox_label(ctx, "glfw", &glfw_lib);
        if (glfw_lib == 0) {
          std::cout << "selected glfw" << "\n";
        }
        /*
        nk_checkbox_label(ctx, "Menu", NULL);
        nk_checkbox_label(ctx, "Border", NULL);
        nk_checkbox_label(ctx, "Resizable", NULL);
        nk_checkbox_label(ctx, "Movable", NULL);
        nk_checkbox_label(ctx, "No Scrollbar", NULL);
        nk_checkbox_label(ctx, "Minimizable", NULL);
        nk_checkbox_label(ctx, "Scale Left", NULL);*/
        nk_tree_pop(ctx);
    }
    if (nk_tree_push(ctx, NK_TREE_TAB, "Selected options", NK_MINIMIZED)) {
        nk_tree_pop(ctx);
    }
    if (nk_button_label (ctx, "compile!")){
        std::cout << "compile" << "\n";
        std::cout << "command " << path_output_file << "\n";
        std::cout << "command " << buf_compile << "\n";

 
        system(path_output_file.c_str());
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



