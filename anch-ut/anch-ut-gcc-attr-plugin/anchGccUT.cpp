#include <iostream>
#include "config.h"
#include "gcc-plugin.h"
#include "tree.h"
#include "cp/cp-tree.h"
#include "diagnostic.h"
#include "plugin.h"

int plugin_is_GPL_compatible;

/*!
 * \c anch_ut::init attribute handler
 *
 * \param node current tree node
 * \param attrName attribute name
 * \param attrArguments attribute args
 * \param flags attribute flags
 * \param no_add_attrs ???
 */
static tree
handleInitAttr(tree* node,
	       tree attrName,
	       tree attrArguments,
	       int flags,
	       bool* no_add_attrs) {
  std::cerr << "Encountered Attribute: anch::" << IDENTIFIER_POINTER(attrName);

  // Print the arguments

  std::string separator = " ";
  for(tree& itrArgument = attrArguments ; itrArgument != NULL_TREE ; itrArgument = TREE_CHAIN(itrArgument)) {
    std::cerr << separator << TREE_STRING_POINTER(TREE_VALUE(itrArgument));
    separator = ", ";
  }

  std::cerr << std::endl;

  // Just return a null tree now.

  return NULL_TREE;
}

/*!
 * \c anch_ut::init attribute definition
 */
static struct attribute_spec init_tests = {
  "init",
  1,
  1,
  false,
  false,
  false,
  false,
  handleInitAttr,
  NULL
};

/*!
 * \c anch_ut::uninit attribute handler
 *
 * \param node current tree node
 * \param attrName attribute name
 * \param attrArguments attribute args
 * \param flags attribute flags
 * \param no_add_attrs ???
 */
static tree
handleUninitAttr(tree* node,
		 tree attrName,
		 tree attrArguments,
		 int flags,
		 bool* no_add_attrs) {
  std::cerr << "Encountered Attribute: anch::" << IDENTIFIER_POINTER(attrName);

  // Print the arguments

  std::string separator = " ";
  for(tree& itrArgument = attrArguments ; itrArgument != NULL_TREE ; itrArgument = TREE_CHAIN(itrArgument)) {
    std::cerr << separator << TREE_STRING_POINTER(TREE_VALUE(itrArgument));
    separator = ", ";
  }

  std::cerr << std::endl;

  // Just return a null tree now.

  return NULL_TREE;
}

/*!
 * \c anch_ut::uinit attribute definition
 */
static struct attribute_spec uninit_tests = {
  "uninit",
  0,
  1,
  true,
  false,
  false,
  false,
  handleUninitAttr,
  NULL
};

/*!
 * \c anch_ut::before attribute handler
 *
 * \param node current tree node
 * \param attrName attribute name
 * \param attrArguments attribute args
 * \param flags attribute flags
 * \param no_add_attrs ???
 */
static tree
handleBeforeAttr(tree* node,
		 tree attrName,
		 tree attrArguments,
		 int flags,
		 bool* no_add_attrs) {
  std::cerr << "Encountered Attribute: anch::" << IDENTIFIER_POINTER(attrName);

  // Print the arguments

  std::string separator = " ";
  for(tree& itrArgument = attrArguments ; itrArgument != NULL_TREE ; itrArgument = TREE_CHAIN(itrArgument)) {
    std::cerr << separator << TREE_STRING_POINTER(TREE_VALUE(itrArgument));
    separator = ", ";
  }

  std::cerr << std::endl;

  // Just return a null tree now.

  return NULL_TREE;
}

/*!
 * \c anch_ut::before attribute definition
 */
static struct attribute_spec before_test = {
  "before",
  0,
  1,
  true,
  false,
  false,
  false,
  handleBeforeAttr,
  NULL
};

/*!
 * \c anch_ut::after attribute handler
 *
 * \param node current tree node
 * \param attrName attribute name
 * \param attrArguments attribute args
 * \param flags attribute flags
 * \param no_add_attrs ???
 */
static tree
handleAfterAttr(tree* node,
		tree attrName,
		tree attrArguments,
		int flags,
		bool* no_add_attrs) {
  std::cerr << "Encountered Attribute: anch::" << IDENTIFIER_POINTER(attrName);

  // Print the arguments

  std::string separator = " ";
  for(tree& itrArgument = attrArguments ; itrArgument != NULL_TREE ; itrArgument = TREE_CHAIN(itrArgument)) {
    std::cerr << separator << TREE_STRING_POINTER(TREE_VALUE(itrArgument));
    separator = ", ";
  }

  std::cerr << std::endl;

  // Just return a null tree now.

  return NULL_TREE;
}

/*!
 * \c anch_ut::after attribute definition
 */
static struct attribute_spec after_test = {
  "after",
  0,
  1,
  true,
  false,
  false,
  false,
  handleAfterAttr,
  NULL
};

/*!
 * \c anch_ut::test attribute handler
 *
 * \param node current tree node
 * \param attrName attribute name
 * \param attrArguments attribute args
 * \param flags attribute flags
 * \param no_add_attrs ???
 */
static tree
handleTestAttr(tree* node,
	       tree attrName,
	       tree attrArguments,
	       int flags,
	       bool* no_add_attrs) {
  std::cerr << "Encountered Attribute: anch::" << IDENTIFIER_POINTER(attrName);

  // Print the arguments

  std::string separator = " ";
  for(tree& itrArgument = attrArguments ; itrArgument != NULL_TREE ; itrArgument = TREE_CHAIN(itrArgument)) {
    std::cerr << separator << TREE_STRING_POINTER(TREE_VALUE(itrArgument));
    separator = ", ";
  }

  std::cerr << std::endl;

  // Just return a null tree now.

  return NULL_TREE;
}

/*!
 * \c anch_ut::test attribute definition
 */
static struct attribute_spec add_test = {
  "test",
  1,
  2,
  false,
  true,
  false,
  false,
  handleTestAttr,
  NULL
};

//    The array of attribute specs passed to register_scoped_attributes must be NULL terminated
attribute_spec anchUTAttrs[] = { init_tests, uninit_tests, before_test, after_test, add_test, NULL };

/*!
 * Register attributes for \c anch_ut
 *
 * \param eventData ?
 * \param userData ?
 */
static void
registerAttributes(void* eventData, void* userData) {
  register_scoped_attributes(anchUTAttrs, "anch_ut", false); // demoScopedAttributes
}

/*!
 * Handle attributes errors ?
 *
 * \param eventData ?
 * \param userData ?
 */
static void
gateCallback(void* eventData, void* userData) {
  // If there has been an error, fall through and let the compiler handle it
  if(errorcount || sorrycount) {
    return;
  }
  std::cerr << "IPA Passes Starting for File: " << main_input_filename << std::endl;
}

/*!
 * Initialize AnCH unit tests GCC plugin
 *
 * \param info the plugin informations
 * \param ver the plugin version
 *
 * \return \c 0
 */
int
plugin_init(plugin_name_args* info, plugin_gcc_version* ver) {
  std::cerr << "Starting Plugin: "<< info->base_name << std::endl;
  register_callback( info->base_name, PLUGIN_ATTRIBUTES, registerAttributes, NULL );
  register_callback( info->base_name, PLUGIN_ALL_IPA_PASSES_START, gateCallback, NULL );
  std::cerr << "Plugin Initialized, attribute registered" << std::endl;
  return 0;
}
