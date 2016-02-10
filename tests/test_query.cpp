#include <fstream>
#include "gtest/gtest.h"
#include "../src/dalm.h"
#include "../src/dalm/buildutil.h"
#include "dalm_test_config.h"
#include "build_dalm_for_test.h"

namespace {
    class QueryTest: public ::testing::Test {
    protected:
        QueryTest(): logger(stderr), test_files(std::string(SOURCE_ROOT) + "/tests/data/test_files.txt"){
            std::ifstream ifs(test_files.c_str());
            std::getline(ifs, arpa);

            arpa = std::string(SOURCE_ROOT) + "/" + arpa;
        }

        virtual void SetUp(){
        }
        virtual void TearDown(){
        }

        DALM::Logger logger;
        std::string test_files;
        std::string arpa;
    };

    TEST_F(QueryTest, test_reverse){
        DALM::Model *m = build_dalm_for_test("rev_query", "reverse", logger, 4);

        std::string test_file = std::string(SOURCE_ROOT) + "/tests/data/kftt-tune.10.ja";
        std::string expected_file = std::string(SOURCE_ROOT) + "/tests/data/test_query_expected.txt";

        std::ifstream efile(expected_file.c_str());
        std::ifstream tfile(test_file.c_str());

        while(!efile.eof()){
            float sentence_prob;
            efile >> sentence_prob;

            std::string line;
            std::getline(tfile, line);
            std::istringstream iss(line);
            DALM::State state;
            m->lm->init_state(state);
            float prob = 0.0;
            while(!iss.eof()){
                std::string word;
                iss >> word;
                DALM::VocabId id = m->vocab->lookup(word.c_str());
                prob += m->lm->query(id, state);
            }
            DALM::VocabId s_tag_end = m->vocab->lookup("</s>");
            prob += m->lm->query(s_tag_end, state);

            EXPECT_NEAR(sentence_prob, prob, 0.001);
        }
        delete m;
    }

    TEST_F(QueryTest, test_bst){
        DALM::Model *m = build_dalm_for_test("bst_query", "bst", logger, 4);

        std::string test_file = std::string(SOURCE_ROOT) + "/tests/data/kftt-tune.10.ja";
        std::string expected_file = std::string(SOURCE_ROOT) + "/tests/data/test_query_expected.txt";

        std::ifstream efile(expected_file.c_str());
        std::ifstream tfile(test_file.c_str());

        while(!efile.eof()){
            float sentence_prob;
            efile >> sentence_prob;

            std::string line;
            std::getline(tfile, line);
            std::istringstream iss(line);
            DALM::State state;
            m->lm->init_state(state);
            float prob = 0.0;
            while(!iss.eof()){
                std::string word;
                iss >> word;
                DALM::VocabId id = m->vocab->lookup(word.c_str());
                float word_prob = m->lm->query(id, state);
                prob += word_prob;
            }
            DALM::VocabId s_tag_end = m->vocab->lookup("</s>");
            prob += m->lm->query(s_tag_end, state);

            EXPECT_NEAR(sentence_prob, prob, 0.001);
        }
        delete m;
    }
}
