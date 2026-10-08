// ============================================================
// CppLib Documentation API
// ============================================================

const Docs = {

    topics: [],


    /**
     * Add a documentation topic.
     *
     * @param {Object} topic
     *
     * Example:
     *
     * Docs.add({
     *     title: "std::vector",
     *     category: "Class",
     *     description: "A dynamic array...",
     *     example: `...`
     * });
     */
    add(topic) {

        if (!topic.title) {
            console.error(
                "Docs.add(): title is required."
            );

            return;
        }


        if (!topic.description) {
            console.error(
                "Docs.add(): description is required."
            );

            return;
        }


        this.topics.push({

            id: topic.id ||
                this.slug(topic.title),

            title: topic.title,

            category:
                topic.category || "General",

            description:
                topic.description,

            example:
                topic.example || "",

            tags:
                topic.tags || []

        });

    },


    /**
     * Generate a URL-friendly ID.
     */
    slug(text) {

        return text
            .toLowerCase()
            .replace(/[^a-z0-9]+/g, "-")
            .replace(/^-|-$/g, "");

    },


    /**
     * Find a topic by ID.
     */
    get(id) {

        return this.topics.find(
            topic => topic.id === id
        );

    },


    /**
     * Search documentation.
     */
    search(query) {

        query =
            query
                .trim()
                .toLowerCase();


        if (!query) {
            return this.topics;
        }


        return this.topics.filter(topic => {

            const content = [

                topic.title,

                topic.category,

                topic.description,

                topic.example,

                ...(topic.tags || [])

            ]
                .join(" ")
                .toLowerCase();


            return content.includes(query);

        });

    }

};


// ============================================================
// YOUR DOCUMENTATION
// ============================================================


// Docs.add({

//     title: "std::vector",

//     category: "Class",

//     description:
//         "std::vector is a sequence container that stores elements in contiguous memory. It provides fast random access and automatically manages its storage.",

//     tags: [
//         "vector",
//         "container",
//         "array",
//         "stl"
//     ],

//     example: `#include <vector>
// #include <iostream>

// int main()
// {
//     std::vector<int> numbers;

//     numbers.push_back(10);
//     numbers.push_back(20);
//     numbers.push_back(30);

//     std::cout << numbers[0];
// }`
// });


// Docs.add({

//     title: "std::string",

//     category: "Class",

//     description:
//         "std::string represents a sequence of characters and provides operations for creating, modifying, searching and comparing strings.",

//     tags: [
//         "string",
//         "text",
//         "stl"
//     ],

//     example: `#include <string>
// #include <iostream>

// int main()
// {
//     std::string name = "CppLib";

//     std::cout << name;
// }`
// });


// Docs.add({

//     title: "std::unique_ptr",

//     category: "Smart Pointer",

//     description:
//         "std::unique_ptr is an exclusive-ownership smart pointer. The managed object is automatically destroyed when the unique_ptr goes out of scope.",

//     tags: [
//         "pointer",
//         "smart pointer",
//         "memory",
//         "raii"
//     ],

//     example: `#include <memory>

// int main()
// {
//     auto value =
//         std::make_unique<int>(42);

//     // value owns the integer
// }`
// });


// Docs.add({

//     title: "std::unordered_map",

//     category: "Container",

//     description:
//         "std::unordered_map stores key-value pairs using a hash table. It provides average constant-time lookup, insertion and removal.",

//     tags: [
//         "map",
//         "hash",
//         "container",
//         "stl"
//     ],

//     example: `#include <unordered_map>
// #include <string>

// int main()
// {
//     std::unordered_map<
//         std::string,
//         int
//     > scores;

//     scores["Alice"] = 100;
//     scores["Bob"] = 85;
// }`
// });


// Docs.add({

//     title: "std::move",

//     category: "Function",

//     description:
//         "std::move converts an expression into an rvalue reference, allowing move constructors and move assignment operators to transfer resources efficiently.",

//     tags: [
//         "move semantics",
//         "rvalue",
//         "performance"
//     ],

//     example: `#include <utility>
// #include <string>

// int main()
// {
//     std::string first = "Hello";

//     std::string second =
//         std::move(first);
// }`
// });


// ============================================================
// DOCUMENTATION RENDERER
// ============================================================

const topicsContainer =
    document.getElementById(
        "topicsContainer"
    );


const searchInput =
    document.getElementById(
        "searchInput"
    );


const topicCount =
    document.getElementById(
        "topicCount"
    );


const noResults =
    document.getElementById(
        "noResults"
    );


// ------------------------------------------------------------
// Escape HTML
// ------------------------------------------------------------

function escapeHTML(value) {

    const element =
        document.createElement("div");

    element.textContent = value;

    return element.innerHTML;

}


// ------------------------------------------------------------
// Category icons
// ------------------------------------------------------------

function getCategoryIcon(category) {

    const icons = {

        "Class": "C",

        "Function": "ƒ",

        "Smart Pointer": "*",

        "Container": "[]",

        "Namespace": "::",

        "Concept": "◇",

        "Example": "</>",

        "Utility": "⚙"

    };


    return icons[category] || "C";

}


// ------------------------------------------------------------
// Render topics
// ------------------------------------------------------------

function renderTopics(query = "") {

    const results =
        Docs.search(query);


    topicsContainer.innerHTML = "";


    topicCount.textContent =
        `${results.length} ${
            results.length === 1
                ? "topic"
                : "topics"
        }`;


    noResults.classList.toggle(
        "hidden",
        results.length !== 0
    );


    results.forEach(topic => {

        const details =
            document.createElement("details");


        details.className =
            "topic";


        details.id =
            topic.id;


        details.innerHTML = `

            <summary>

                <div class="topic-icon">
                    ${getCategoryIcon(
                        topic.category
                    )}
                </div>


                <div>

                    <div class="topic-title">

                        ${escapeHTML(
                            topic.title
                        )}

                        <span
                            class="topic-category"
                        >
                            ${escapeHTML(
                                topic.category
                            )}
                        </span>

                    </div>

                </div>

            </summary>


            <div class="topic-content">

                <p>
                    ${topic.description}
                </p>


                ${
                    topic.tags?.length
                        ? `
                            <div class="topic-tags">

                                ${topic.tags
                                    .map(tag => `
                                        <span>
                                            ${escapeHTML(tag)}
                                        </span>
                                    `)
                                    .join("")}

                            </div>
                          `
                        : ""
                }


                ${
                    topic.example
                        ? `

                            <h4>
                                Example
                            </h4>

                            <div
                                class="code-block"
                            >

                                <pre><code>${escapeHTML(
                                    topic.example
                                )}</code></pre>

                            </div>

                          `
                        : ""
                }

            </div>
        `;


        topicsContainer.appendChild(
            details
        );

    });

}


// ============================================================
// LIVE SEARCH
// ============================================================

searchInput.addEventListener(
    "input",
    () => {

        renderTopics(
            searchInput.value
        );

    }
);


// ============================================================
// CTRL + K
// ============================================================

document.addEventListener(
    "keydown",
    event => {

        if (
            (event.ctrlKey ||
             event.metaKey) &&

            event.key.toLowerCase() === "k"
        ) {

            event.preventDefault();

            searchInput.focus();

        }

    }
);


// ============================================================
// INITIAL RENDER
// ============================================================


const extendAllButton = document.getElementById("extend_all");

extendAllButton.addEventListener("click", () => {

    const topics = document.querySelectorAll(
        "#topicsContainer details"
    );

    if (!topics.length) return;

    const allOpen = [...topics].every(
        topic => topic.open
    );

    topics.forEach(topic => {
        topic.open = !allOpen;
    });

    extendAllButton.textContent =
        allOpen ? "Expand All" : "Collapse All";
});