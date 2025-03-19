1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines when a command's output is fully received by using a protocol-specific mechanism to mark the end of the transmission. In the provided code, the server sends an EOF character `RDSH_EOF_CHAR` to indicate the end of the command's output. The client checks for this character after each `recv()` call to determine if the transmission is complete.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol can define message boundaries using delimiters (e.g., \n or RDSH_EOF_CHAR) or length-prefixed messages (sending the message size before the data). Without proper handling, TCP's stream nature can cause incomplete messages, blocking, or data corruption. 
n.
3. Describe the general differences between stateful and stateless protocols.

Stateful protocols maintain client state on the server, enabling efficient long-lived connections but adding complexity and vulnerability to crashes. Stateless protocols treat each request independently, simplifying implementation and scaling but requiring additional mechanisms (e.g., cookies) for state simulation. Stateful protocols are better for persistent interactions, while stateless protocols excel in scalability and resilience.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

It's not the UDP is "unreliable" it just doesn't ALWAYS guarantee delivery or order. TCP can also track the state.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

We use the socket api defined in `sockets/h`.