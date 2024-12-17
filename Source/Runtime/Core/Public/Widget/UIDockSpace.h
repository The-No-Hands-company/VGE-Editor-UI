#include <gtest/gtest.h>
#include "LogisticRegression.h"
#include "DataSet.h"
#include "Optimizer.h"
#include <random>

namespace VGE::AI::ML::Tests {

class LogisticRegressionTest : public ::testing::Test {
protected:
    void SetUp() override {
        model = std::make_unique<LogisticRegression>();
    }

    void TearDown() override {
        model.reset();
    }

    // Helper function to generate synthetic binary classification data
    DataSet GenerateBinaryData(size_t numSamples, size_t numFeatures) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> dist(0.0f, 1.0f);
        std::uniform_real_distribution<float> noise(-0.1f, 0.1f);

        DataSet dataset;
        std::vector<float> weights(numFeatures);
        float bias = dist(gen);

        // Generate random weights
        for (size_t i = 0; i < numFeatures; ++i) {
            weights[i] = dist(gen);
        }

        // Generate samples
        for (size_t i = 0; i < numSamples; ++i) {
            std::vector<float> features(numFeatures);
            float z = bias;

            for (size_t j = 0; j < numFeatures; ++j) {
                features[j] = dist(gen);
                z += features[j] * weights[j];
            }

            // Add noise and apply sigmoid
            z += noise(gen);
            float p = 1.0f / (1.0f + std::exp(-z));
            float label = p > 0.5f ? 1.0f : 0.0f;

            dataset.AddSample(features, {label});
        }

        return dataset;
    }

    // Helper function to generate synthetic multi-class data
    DataSet GenerateMultiClassData(size_t numSamples, size_t numFeatures, size_t numClasses) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<float> dist(0.0f, 1.0f);

        DataSet dataset;
        std::vector<std::vector<float>> weights(numClasses, std::vector<float>(numFeatures));
        std::vector<float> biases(numClasses);

        // Generate random weights and biases for each class
        for (size_t c = 0; c < numClasses; ++c) {
            for (size_t j = 0; j < numFeatures; ++j) {
                weights[c][j] = dist(gen);
            }
            biases[c] = dist(gen);
        }

        // Generate samples
        for (size_t i = 0; i < numSamples; ++i) {
            std::vector<float> features(numFeatures);
            std::vector<float> logits(numClasses);

            // Generate features
            for (size_t j = 0; j < numFeatures; ++j) {
                features[j] = dist(gen);
            }

            // Compute logits for each class
            for (size_t c = 0; c < numClasses; ++c) {
                logits[c] = biases[c];
                for (size_t j = 0; j < numFeatures; ++j) {
                    logits[c] += features[j] * weights[c][j];
                }
            }

            // Find class with maximum logit
            size_t maxClass = 0;
            float maxLogit = logits[0];
            for (size_t c = 1; c < numClasses; ++c) {
                if (logits[c] > maxLogit) {
                    maxLogit = logits[c];
                    maxClass = c;
                }
            }

            dataset.AddSample(features, {static_cast<float>(maxClass)});
        }

        return dataset;
    }

    std::unique_ptr<LogisticRegression> model;
};

TEST_F(LogisticRegressionTest, BinaryClassification) {
    // Generate binary classification data
    DataSet trainData = GenerateBinaryData(1000, 5);
    DataSet testData = GenerateBinaryData(200, 5);
    
    // Train model
    EXPECT_TRUE(model->Train(trainData));
    
    // Evaluate
    float accuracy = model->Evaluate(testData);
    EXPECT_GT(accuracy, 0.7f); // Should perform better than random
    
    // Test single prediction
    auto features = testData.GetFeatures()[0];
    auto prediction = model->Predict(features);
    EXPECT_FALSE(prediction.empty());
    EXPECT_TRUE(prediction[0] == 0.0f || prediction[0] == 1.0f);
}

TEST_F(LogisticRegressionTest, MultiClassClassification) {
    // Enable multi-class classification
    model->EnableMultiClass(true);
    
    // Generate multi-class data
    const size_t NUM_CLASSES = 3;
    DataSet trainData = GenerateMultiClassData(1000, 5, NUM_CLASSES);
    DataSet testData = GenerateMultiClassData(200, 5, NUM_CLASSES);
    
    // Train model
    EXPECT_TRUE(model->Train(trainData));
    
    // Evaluate
    float accuracy = model->Evaluate(testData);
    EXPECT_GT(accuracy, 0.5f); // Should perform better than random for 3 classes
    
    // Test probabilities
    auto features = testData.GetFeatures()[0];
    auto probs = model->GetProbabilities(features);
    EXPECT_EQ(probs.size(), NUM_CLASSES);
    
    float sum = 0.0f;
    for (float p : probs) {
        EXPECT_GE(p, 0.0f);
        EXPECT_LE(p, 1.0f);
        sum += p;
    }
    EXPECT_NEAR(sum, 1.0f, 1e-5f);
}

TEST_F(LogisticRegressionTest, Optimization) {
    // Test different optimizers
    std::vector<std::shared_ptr<Optimizer>> optimizers = {
        std::make_shared<SGDOptimizer>(0.01f),
        std::make_shared<AdamOptimizer>(0.01f),
        std::make_shared<RMSpropOptimizer>(0.01f)
    };
    
    DataSet trainData = GenerateBinaryData(1000, 5);
    DataSet testData = GenerateBinaryData(200, 5);
    
    for 