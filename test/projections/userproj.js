function projection(e, s)
{
    switch (e.type) {
    case "UserCreated":
        s = e.payload;
        break;
    case "UserEmailChanged":
        s.email = e.payload.newEmail;
        break;
    default:
        throw "Unknown event type: " + e.type;
    }
    return s;
}
