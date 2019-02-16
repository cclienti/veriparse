#include "../helpers/helpers.hpp"
#include <veriparse/parser/verilog.hpp>
#include <veriparse/logger/logger.hpp>
#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/generators/dot_generator.hpp>
#include <veriparse/importers/yaml_importer.hpp>
#include <gtest/gtest.h>
#include <fstream>
#include <string>

using namespace Veriparse;

static TestHelpers test_helpers("lib/test/parser/testcases/");


#define TEST_CORE                                                                      \
	ENABLE_LOGGER;                                                                      \
                                                                                       \
	/* Parse the module */                                                              \
	Parser::Verilog verilog;                                                            \
	verilog.parse(test_helpers.get_verilog_filename(test_name));                        \
	AST::Node::Ptr source = verilog.get_source();                                       \
	ASSERT_TRUE(source != nullptr);                                                     \
	test_helpers.render_node_to_yaml_file(source, test_string + "_parsed.yaml");        \
	test_helpers.render_node_to_dot_file(source, test_string + "_parsed.dot");          \
                                                                                       \
	/* Full cloning and is_equal test */                                                \
	AST::Node::Ptr source_cloned = source->clone();                                     \
	ASSERT_TRUE(source->is_equal(source_cloned));                                       \
	test_helpers.render_node_to_dot_file(source_cloned, test_string + "_cloned.dot");   \
	test_helpers.render_node_to_yaml_file(source_cloned, test_string + "_cloned.yaml"); \
                                                                                       \
	/* Import the YAML reference */                                                     \
	AST::Node::Ptr source_ref = Importers::YAMLImporter().import                        \
		(test_helpers.get_yaml_filename(test_name).c_str());                             \
	ASSERT_TRUE(source_ref != nullptr);                                                 \
                                                                                       \
	/* Check parsed against reference */                                                \
	ASSERT_TRUE(source->is_equal(*source_ref, false))


TEST(VerilogParserTest, module0)        {TEST_CORE;}
TEST(VerilogParserTest, module1)        {TEST_CORE;}
TEST(VerilogParserTest, module2)        {TEST_CORE;}
TEST(VerilogParserTest, module3)        {TEST_CORE;}
TEST(VerilogParserTest, localparam0)    {TEST_CORE;}
TEST(VerilogParserTest, parameter0)     {TEST_CORE;}
TEST(VerilogParserTest, parameter1)     {TEST_CORE;}
TEST(VerilogParserTest, defparam0)      {TEST_CORE;}
TEST(VerilogParserTest, defparam1)      {TEST_CORE;}
TEST(VerilogParserTest, defparam2)      {TEST_CORE;}
TEST(VerilogParserTest, defparam3)      {TEST_CORE;}
TEST(VerilogParserTest, translate_off0) {TEST_CORE;}
TEST(VerilogParserTest, translate_off1) {TEST_CORE;}
TEST(VerilogParserTest, pragma0)        {TEST_CORE;}
TEST(VerilogParserTest, pragma1)        {TEST_CORE;}
TEST(VerilogParserTest, net0)           {TEST_CORE;}
TEST(VerilogParserTest, net1)           {TEST_CORE;}
TEST(VerilogParserTest, net2)           {TEST_CORE;}
TEST(VerilogParserTest, net3)           {TEST_CORE;}
TEST(VerilogParserTest, var0)           {TEST_CORE;}
TEST(VerilogParserTest, var1)           {TEST_CORE;}
TEST(VerilogParserTest, var2)           {TEST_CORE;}
TEST(VerilogParserTest, expression0)    {TEST_CORE;}
TEST(VerilogParserTest, expression1)    {TEST_CORE;}
TEST(VerilogParserTest, expression2)    {TEST_CORE;}
TEST(VerilogParserTest, expression3)    {TEST_CORE;}
TEST(VerilogParserTest, expression4)    {TEST_CORE;}
TEST(VerilogParserTest, expression5)    {TEST_CORE;}
TEST(VerilogParserTest, expression6)    {TEST_CORE;}
TEST(VerilogParserTest, expression7)    {TEST_CORE;}
TEST(VerilogParserTest, expression8)    {TEST_CORE;}
TEST(VerilogParserTest, expression9)    {TEST_CORE;}
TEST(VerilogParserTest, expression10)   {TEST_CORE;}
TEST(VerilogParserTest, expression11)   {TEST_CORE;}
TEST(VerilogParserTest, expression12)   {TEST_CORE;}
TEST(VerilogParserTest, expression13)   {TEST_CORE;}
TEST(VerilogParserTest, expression14)   {TEST_CORE;}
TEST(VerilogParserTest, expression15)   {TEST_CORE;}
TEST(VerilogParserTest, expression16)   {TEST_CORE;}
TEST(VerilogParserTest, expression17)   {TEST_CORE;}
TEST(VerilogParserTest, expression18)   {TEST_CORE;}
TEST(VerilogParserTest, expression19)   {TEST_CORE;}
TEST(VerilogParserTest, expression20)   {TEST_CORE;}
TEST(VerilogParserTest, expression21)   {TEST_CORE;}
TEST(VerilogParserTest, expression22)   {TEST_CORE;}
TEST(VerilogParserTest, expression23)   {TEST_CORE;}
TEST(VerilogParserTest, expression24)   {TEST_CORE;}
TEST(VerilogParserTest, expression25)   {TEST_CORE;}
TEST(VerilogParserTest, expression26)   {TEST_CORE;}
TEST(VerilogParserTest, expression27)   {TEST_CORE;}
TEST(VerilogParserTest, expression28)   {TEST_CORE;}
TEST(VerilogParserTest, expression29)   {TEST_CORE;}
TEST(VerilogParserTest, expression30)   {TEST_CORE;}
TEST(VerilogParserTest, expression31)   {TEST_CORE;}
TEST(VerilogParserTest, expression32)   {TEST_CORE;}
TEST(VerilogParserTest, expression33)   {TEST_CORE;}
TEST(VerilogParserTest, expression34)   {TEST_CORE;}
TEST(VerilogParserTest, expression35)   {TEST_CORE;}
TEST(VerilogParserTest, expression36)   {TEST_CORE;}
TEST(VerilogParserTest, expression37)   {TEST_CORE;}
TEST(VerilogParserTest, expression38)   {TEST_CORE;}
TEST(VerilogParserTest, expression39)   {TEST_CORE;}
TEST(VerilogParserTest, expression40)   {TEST_CORE;}
TEST(VerilogParserTest, expression41)   {TEST_CORE;}
TEST(VerilogParserTest, expression42)   {TEST_CORE;}
TEST(VerilogParserTest, expression43)   {TEST_CORE;}
TEST(VerilogParserTest, expression44)   {TEST_CORE;}
TEST(VerilogParserTest, expression45)   {TEST_CORE;}
TEST(VerilogParserTest, expression46)   {TEST_CORE;}
TEST(VerilogParserTest, expression47)   {TEST_CORE;}
TEST(VerilogParserTest, intconst0)      {TEST_CORE;}
TEST(VerilogParserTest, intconst1)      {TEST_CORE;}
TEST(VerilogParserTest, intconst2)      {TEST_CORE;}
TEST(VerilogParserTest, intconst3)      {TEST_CORE;}
TEST(VerilogParserTest, intconst4)      {TEST_CORE;}
TEST(VerilogParserTest, intconst5)      {TEST_CORE;}
TEST(VerilogParserTest, intconst6)      {TEST_CORE;}
TEST(VerilogParserTest, intconst7)      {TEST_CORE;}
TEST(VerilogParserTest, intconst8)      {TEST_CORE;}
TEST(VerilogParserTest, realconst0)     {TEST_CORE;}
TEST(VerilogParserTest, realconst1)     {TEST_CORE;}
TEST(VerilogParserTest, realconst2)     {TEST_CORE;}
TEST(VerilogParserTest, realconst3)     {TEST_CORE;}
TEST(VerilogParserTest, stringconst0)   {TEST_CORE;}
TEST(VerilogParserTest, stringconst1)   {TEST_CORE;}
TEST(VerilogParserTest, stringconst2)   {TEST_CORE;}
TEST(VerilogParserTest, always0)        {TEST_CORE;}
TEST(VerilogParserTest, always1)        {TEST_CORE;}
TEST(VerilogParserTest, always2)        {TEST_CORE;}
TEST(VerilogParserTest, always3)        {TEST_CORE;}
TEST(VerilogParserTest, always4)        {TEST_CORE;}
TEST(VerilogParserTest, always5)        {TEST_CORE;}
TEST(VerilogParserTest, always6)        {TEST_CORE;}
TEST(VerilogParserTest, cond0)          {TEST_CORE;}
TEST(VerilogParserTest, if0)            {TEST_CORE;}
TEST(VerilogParserTest, if1)            {TEST_CORE;}
TEST(VerilogParserTest, if2)            {TEST_CORE;}
TEST(VerilogParserTest, case0)          {TEST_CORE;}
TEST(VerilogParserTest, case1)          {TEST_CORE;}
TEST(VerilogParserTest, casex0)         {TEST_CORE;}
TEST(VerilogParserTest, casez0)         {TEST_CORE;}
TEST(VerilogParserTest, for0)           {TEST_CORE;}
TEST(VerilogParserTest, for1)           {TEST_CORE;}
TEST(VerilogParserTest, while0)         {TEST_CORE;}
TEST(VerilogParserTest, while1)         {TEST_CORE;}
TEST(VerilogParserTest, event0)         {TEST_CORE;}
TEST(VerilogParserTest, event1)         {TEST_CORE;}
TEST(VerilogParserTest, wait0)          {TEST_CORE;}
TEST(VerilogParserTest, wait1)          {TEST_CORE;}
TEST(VerilogParserTest, forever0)       {TEST_CORE;}
TEST(VerilogParserTest, forever1)       {TEST_CORE;}
TEST(VerilogParserTest, initial0)       {TEST_CORE;}
TEST(VerilogParserTest, initial1)       {TEST_CORE;}
TEST(VerilogParserTest, block0)         {TEST_CORE;}
TEST(VerilogParserTest, namedblock0)    {TEST_CORE;}
TEST(VerilogParserTest, fork0)          {TEST_CORE;}
TEST(VerilogParserTest, assignment0)    {TEST_CORE;}
TEST(VerilogParserTest, assignment1)    {TEST_CORE;}
TEST(VerilogParserTest, disable0)       {TEST_CORE;}
TEST(VerilogParserTest, function0)      {TEST_CORE;}
TEST(VerilogParserTest, function1)      {TEST_CORE;}
TEST(VerilogParserTest, function2)      {TEST_CORE;}
TEST(VerilogParserTest, function3)      {TEST_CORE;}
TEST(VerilogParserTest, functioncall0)  {TEST_CORE;}
TEST(VerilogParserTest, task0)          {TEST_CORE;}
TEST(VerilogParserTest, task1)          {TEST_CORE;}
TEST(VerilogParserTest, task2)          {TEST_CORE;}
TEST(VerilogParserTest, taskcall0)      {TEST_CORE;}
TEST(VerilogParserTest, systemcall0)    {TEST_CORE;}
TEST(VerilogParserTest, instance0)      {TEST_CORE;}
TEST(VerilogParserTest, instance1)      {TEST_CORE;}
TEST(VerilogParserTest, instance2)      {TEST_CORE;}
TEST(VerilogParserTest, instance3)      {TEST_CORE;}
TEST(VerilogParserTest, instance4)      {TEST_CORE;}
TEST(VerilogParserTest, instance5)      {TEST_CORE;}
TEST(VerilogParserTest, instance6)      {TEST_CORE;}
TEST(VerilogParserTest, instance7)      {TEST_CORE;}
TEST(VerilogParserTest, instance8)      {TEST_CORE;}
TEST(VerilogParserTest, instance9)      {TEST_CORE;}
TEST(VerilogParserTest, generate0)      {TEST_CORE;}
TEST(VerilogParserTest, generate1)      {TEST_CORE;}
TEST(VerilogParserTest, generate2)      {TEST_CORE;}
TEST(VerilogParserTest, generate3)      {TEST_CORE;}
TEST(VerilogParserTest, width0)         {TEST_CORE;}
TEST(VerilogParserTest, width1)         {TEST_CORE;}
TEST(VerilogParserTest, bidir0)         {TEST_CORE;}

// Preprocessor directives test
TEST(VerilogParserTest, vpp_line0)            {TEST_CORE;}
TEST(VerilogParserTest, vpp_default_nettype0) {TEST_CORE;}
TEST(VerilogParserTest, vpp_default_nettype1) {TEST_CORE;}
TEST(VerilogParserTest, vpp_default_nettype2) {TEST_CORE;}
TEST(VerilogParserTest, vpp_default_nettype3) {TEST_CORE;}
TEST(VerilogParserTest, vpp_default_nettype4) {TEST_CORE;}
TEST(VerilogParserTest, vpp_default_nettype5) {TEST_CORE;}
TEST(VerilogParserTest, vpp_default_nettype6) {TEST_CORE;}
TEST(VerilogParserTest, vpp_default_nettype7) {TEST_CORE;}
